queue = {head = 1, tail = 1}

printk = function(...)
	local t = {}
	for _,v in pairs{...} do
		table.insert(t, tostring(v))
	end
	table.insert(t, '\n')
	queue[queue.tail] = table.concat(t, '\t')
	queue.tail = queue.tail + 1
end

showk = function()
	if queue.head >= queue.tail then
		return nil
	end

	local s = queue[queue.head]
	queue[queue.head] = nil
	queue.head = queue.head + 1
	return s
end
