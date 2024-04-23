local function doDash(pid, distance)
	if distance <= 0 then return end

	local player = Player(pid)
	local dir = player:getDirection()
	local position = player:getPosition()

	player:isMovementBlocked(true)

	--Determine next position
	position:getNextPosition(dir)

	--If the Player can occupy the Tile on the next position
	local tile = Tile(position)
	if tile and
		not tile:hasProperty(CONST_PROP_BLOCKSOLID) and
		not tile:hasProperty(CONST_PROP_IMMOVABLEBLOCKSOLID) and
		player:getPathTo(position) then

		--Move Player to new position
		player:teleportTo(position, false)

		--And queue the next tick of the dash if we still have distance to cover
		if distance > 0 then
			addEvent(doDash, 50, pid, distance - 1)
			return
		end
	end

	player:isMovementBlocked(false)
end

local WINDUP_DELAY = 150
local DASH_DISTANCE = 5
local callback = EventCallback

callback.onTurn = function(self, direction)
	self:setDirection(direction)
	addEvent(doDash, WINDUP_DELAY, self:getId(), DASH_DISTANCE)
end

callback:register()
