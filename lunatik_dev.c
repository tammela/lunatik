#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include<linux/uaccess.h>

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#define ENSURE(x, action, ...) if (!(x)) { printk(__VA_ARGS__); action; }

#define CTL_CLASS   "lua-ctl"
#define CTL_DEVICE  "lua/ctl"

#define INST_CLASS  "lua-inst"
#define INST_DEVICE "lua/%d"

static struct lunatik_s {
	dev_t         dev,    inst_dev;
	struct cdev   cdev,   inst_cdev;
	struct class *class, *inst_class;

	size_t default_bufsize;
	struct lunatik_inst_s {
		struct device *device;
		lua_State *L;
		char  *buf;
		size_t buf_max;
	} ctl, inst[64];
	uint64_t mask;
} g_lunatik;

static int lunatik_newstate(lua_State *L);
static int lunatik_delstate(lua_State *L);

static const struct luaL_Reg lunatik_reg[] = { 
    {"newstate", lunatik_newstate},
    {"delstate", lunatik_delstate},
    {NULL      , NULL},
};

static int
lunatik_open(struct inode *inode, struct file *filp)
{
	filp->private_data = (imajor(inode) == MAJOR(g_lunatik.dev))
	  ? &g_lunatik.ctl
	  : &g_lunatik.inst[iminor(inode)];
	try_module_get(THIS_MODULE);
	return 0;
}

static int
lunatik_release(struct inode *inode, struct file *filp)
{
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t
lunatik_read(struct file *filp, char *p, size_t n, loff_t *o)
{
	struct lunatik_inst_s *inst = filp->private_data;
	const char *lp;
	size_t m;
	
	int top = lua_gettop(inst->L);
	lua_getglobal(inst->L, "showk");
	if ((lua_pcall(inst->L, 0, 1, 0) != LUA_OK) ||
	    (lua_type(inst->L, -1) != LUA_TSTRING))
		return 0;

	lp = luaL_checklstring(inst->L, -1, &m);
	if (m > n)
		return -EINVAL;
	lua_settop(inst->L, top);
	return m - copy_to_user(p, lp, m);
}

static ssize_t
lunatik_write(struct file *filp, const char *p, size_t n, loff_t *o)
{
	struct lunatik_inst_s *inst = filp->private_data;
	//struct lunatik_s      *lun  = &g_lunatik;
	int top, m;

	ENSURE
	  (n < inst->buf_max
	  ,return -EINVAL
	  ,"write chunch too large\n");

	m = copy_from_user(inst->buf, p, n);
	inst->buf[n] = '\0';

	top = lua_gettop(inst->L);
	if (luaL_dostring(inst->L, inst->buf) != 0) {
		printk(KERN_INFO "%s", luaL_checkstring(inst->L, -1));
		lua_settop(inst->L, top);
	}
	return n;
}

static struct file_operations ctl_fops = {
	.open    = lunatik_open,
	.release = lunatik_release,
	.read    = lunatik_read,
	.write   = lunatik_write,
};

static struct file_operations inst_fops = {
	.open    = lunatik_open,
	.release = lunatik_release,
	.read    = lunatik_read,
	.write   = lunatik_write,
};


int lunatik_dev_init(void)
{
	struct lunatik_s *lun = &g_lunatik;
	lun->mask = 0;
	lun->default_bufsize = 64 * 1024; // 64k

	/* ctl */
	ENSURE
	  ((alloc_chrdev_region(&lun->dev, 0, 1, CTL_CLASS)) >= 0
	  ,goto out0
	  ,KERN_INFO "failed to allocate a character device.\n");

	cdev_init(&lun->cdev, &ctl_fops);

	ENSURE
	  ((cdev_add(&lun->cdev, lun->dev, 1)) >= 0
	  ,goto out1
	  ,KERN_INFO "failed to add the device to the system\n");
	ENSURE
	  ((lun->class = class_create(THIS_MODULE, CTL_CLASS)) != NULL
	  ,goto out1
	  ,KERN_INFO "failed to create module class\n");
	ENSURE
	  ((lun->ctl.device = device_create(lun->class, NULL, lun->dev, NULL, CTL_DEVICE)) != NULL
	  ,goto out2
	  ,KERN_INFO "failed to create device\n");

	/* inst */
	ENSURE
	  ((alloc_chrdev_region(&lun->inst_dev, 0, 64, INST_CLASS)) >= 0
	  ,goto out0
	  ,KERN_INFO "failed to allocate a character device.\n");

	cdev_init(&lun->inst_cdev, &inst_fops);

	ENSURE
	  ((cdev_add(&lun->inst_cdev, lun->inst_dev, 64)) >= 0
	  ,goto out1
	  ,KERN_INFO "failed to add the device to the system\n");
	ENSURE
	  ((lun->inst_class = class_create(THIS_MODULE, INST_CLASS)) != NULL
	  ,goto out1
	  ,KERN_INFO "failed to create module class\n");

	lun->ctl.buf_max = lun->default_bufsize;
	lun->ctl.buf     = kmalloc(lun->ctl.buf_max, GFP_KERNEL);

	ENSURE
	  (lun->ctl.buf != NULL
	  ,goto out2
	  ,KERN_INFO "failed to allocate buffer memory\n");

	lun->ctl.L = luaL_newstate();
	luaL_openlibs(lun->ctl.L);

	luaL_newlib(lun->ctl.L, lunatik_reg);
	lua_setglobal(lun->ctl.L, "lunatik");

	return 0;

out2:	class_destroy(lun->class);
out1:	unregister_chrdev_region(lun->dev, 1);
out0:	return -1;
}

void lunatik_dev_fini(void)
{
	struct lunatik_s *lun = &g_lunatik;

	device_destroy(lun->class, lun->dev);
	class_destroy(lun->class);
	unregister_chrdev_region(lun->dev, 1);
}

static int ffz64(uint64_t x)
{
	return __builtin_ffsll(~x) - 1;
}

static int lunatik_newstate(lua_State *L)
{
	int i;
	struct lunatik_s      *lun = &g_lunatik;
	struct lunatik_inst_s *inst= 0;

	ENSURE
	  (lun->mask != -1ull
	  ,return -1
	  ,"failed to allocate a new state\n");

	i = ffz64(lun->mask);
	inst = &lun->inst[i];

	inst->device = device_create(lun->inst_class, lun->ctl.device,
		lun->inst_dev+i, NULL, INST_DEVICE, i);

	ENSURE
	  (inst->device != NULL
	  ,goto out0
	  ,KERN_INFO "device creation failed\n");

	inst->buf_max = lun->default_bufsize;
	inst->buf     = kmalloc(inst->buf_max, GFP_KERNEL);

	inst->L = luaL_newstate();
	luaL_openlibs(inst->L);

	lun->mask |= (1<<i);
	lua_pushinteger(L, i);
	return 1;

out0:	lua_pushliteral(L, "state creation failed\n");
	lua_error(L);
	return -1;
}

static int lunatik_delstate(lua_State *L)
{
	int i = luaL_checkinteger(L, 1);
	struct lunatik_s      *lun = &g_lunatik;
	struct lunatik_inst_s *inst= 0;
	int exists = lun->mask & (1<<i);

	if (exists) {
		inst = &lun->inst[i];
		kfree(inst->buf);
		lun->mask &= ~(1 << i);
		lua_close(inst->L);
		device_del(inst->device);
	}
	lua_pushboolean(L, exists);
	return 1;
}
