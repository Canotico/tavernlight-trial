/*
Creating an empty Player through 'new Player(nullptr)' will cause a memory 
leak because the new object is not being deleted manually afterwards.

Creating it without the 'new' keyword will ensure the Player object will
be cleaned up as soon as the function is done.
*/

void Game::addItemToPlayer(const std::string& recipient, uint16_t itemId)
{
	Player* player = g_game.getPlayerByName(recipient);
	if (!player) 
	{
		player = &Player(nullptr);
		if (!IOLoginData::loadPlayerByName(player, recipient)) 
		{
			return;
		}
	}
	
	Item* item = Item::CreateItem(itemId);
	if (!item) 
	{
		return;
	}
	
	g_game.internalAddItem(player->getInbox(), item, INDEX_WHEREEVER, FLAG_NOLIMIT);
	
	if (player->isOffline()) 
	{
		IOLoginData::savePlayer(player);
	}
}