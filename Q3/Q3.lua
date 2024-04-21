--[[
The function removes a Player from its current party.

A Player can only be part of a single Party at most so we can derive the Party 
information just from the Player we wish to remove from it, no need to pass extra 
Player data in the parameters.

There were a lot of repeated calls to ‘Player(membername)’ which is inefficient. 
These calls have been condensed into the single ‘player’ variable.

Added an early out in the event that the Player doesn’t currently belong to any party.
]]

function remove_party_member(playerId)
	-- single definition of player
	local player = Player(playerId)
	local party = player:getParty()
	
	-- make sure the player does belong to a party
	if party == nil then return end
	
	for k,v in pairs(party:getMembers()) do
		-- as soon as we find the Player we remove them and then stop looping
		if v == player then
			party:removeMember(player)
			break
		end
	end
end