--[[
The variable ‘result’ wasn’t defined anywhere, so the call to ‘getString()’ would’ve failed 
and ‘guildName’ would ultimately be nil

Additionally, no iteration was being done on the query result (which should be a table), 
which also makes it impossible to properly print the guilds’ names.

Renamed ‘resultId’ to ‘guildsFound’ and added a for loop to iterate through the guilds that 
matched the criteria and print their names.
]]

function printSmallGuildNames(memberCount)
	-- this method is supposed to print names of all guilds that have less than memberCount max members
	local selectGuildQuery = "SELECT name FROM guilds WHERE max_members < %d;"
	-- renamed "resultId" to "guildsFound" to better convey its purpose
	local guildsFound = db.storeQuery(string.format(selectGuildQuery, memberCount))
	-- we want to loop through guildsFound to print the names of each guild
	for k, v in pairs(guildsFound) do
		print(v.getString("name"))
	end
end