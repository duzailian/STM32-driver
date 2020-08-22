1. 将nRF24L01分为master和slave节点
	master节点:
		1. 节点一直处于接收状态(功耗较高),收到数据时通过callback向上层输出数据
		2. 若上层调用write接口执行,则将数据加入缓存队列,待收到slave节点发送的数据后
		   作为ACK payload发送给slave节点
	slave节点:
		1. slave节点作为数据发送的发起方,可随时发送数据给master节点
		2. slave节点可以在必要时候将nRF24L01设置为poweroff状态并将MCU休眠
demo:
