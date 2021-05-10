ccflags-y += -D_LUNATIK -D_KERNEL -I$(src) -D_CONFIG_FULL_PANIC
asflags-y += -D_LUNATIK -D_KERNEL

ifeq ($(ARCH), $(filter $(ARCH),i386 x86))
	AFLAGS_setjmp.o := -D_REGPARM
endif

ifeq ($(ARCH), mips)
	ifdef CONFIG_64BIT
		ifdef CONFIG_MIPS32_O32
			AFLAGS_setjmp.o += -D__mips_o32
		else
			ifdef CONFIG_MIPS32_N32
				AFLAGS_setjmp.o += -D__mips_n32
			else
				AFLAGS_setjmp.o += -D__mips_n64
			endif
		endif
		AFLAGS_setjmp.o += -D_MIPS_ISA_MIPS64 \
			-D_MIPS_ISA=_MIPS_ISA_MIPS64
        else
		AFLAGS_setjmp.o += -D__mips_o32 -D_MIPS_ISA_MIPS32 \
			-D_MIPS_ISA=_MIPS_ISA_MIPS32
        endif
endif

obj-$(CONFIG_LUNATIK) += lunatik.o

lunatik-objs += lua/lapi.o lua/lcode.o lua/lctype.o lua/ldebug.o lua/ldo.o \
	 lua/ldump.o lua/lfunc.o lua/lgc.o lua/llex.o lua/lmem.o \
	 lua/lobject.o lua/lopcodes.o lua/lparser.o lua/lstate.o \
	 lua/lstring.o lua/ltable.o lua/ltm.o \
	 lua/lundump.o lua/lvm.o lua/lzio.o lua/lauxlib.o lua/lbaselib.o \
	 lua/lbitlib.o lua/lcorolib.o lua/ldblib.o lua/lstrlib.o \
	 lua/ltablib.o lua/lutf8lib.o lua/loslib.o lua/lmathlib.o lua/linit.o \
	 lua/loadlib.o \
	 arch/$(ARCH)/setjmp.o util/modti3.o lunatik_core.o lunatik_dev.o

ifeq ($(shell [ "${VERSION}" -lt "4" ] && [ "${VERSION}${PATCHLEVEL}" -lt "312" ] && echo y),y)
	lunatik-objs += util/div64.o
endif
