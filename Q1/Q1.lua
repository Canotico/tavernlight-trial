--[[
The ‘onLogout’ function receives a ‘Player object’ that is unsafe to send as a parameter to the ‘addEvent’ call. 

Sending an Id, which is a plain number and therefore safe, will give ‘releaseStorage’ enough information to 
reconstruct the Player object set their storage value.
]]

-- receive the Player Id as parameter instead
local function releaseStorage(pid)
	local player = Player(pid)
	player:setStorageValue(1000, -1)
end

function onLogout(player)
	if player:getStorageValue(1000) == 1 then
		-- sending the Player Id is safe
		addEvent(releaseStorage, 1000, player:getId())
	end
	return true
end