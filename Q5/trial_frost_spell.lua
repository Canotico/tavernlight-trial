local SPELL_REPEAT_COUNT = 4

function spellCallback(cid, position, count)
	--66.66% chance to spawn effect on tile
	if math.random(0,2) < 2 then
		doAreaCombat(cid, COMBAT_ICEDAMAGE, position, 0, -100, -100, CONST_ME_ICETORNADO)
	end

	--wait to repeat effect
	if count <= SPELL_REPEAT_COUNT then
		addEvent(spellCallback, math.random(500, 750), cid, position, count + 1)
	end
end

function onTargetTile(creature, position)
	spellCallback(creature:getId(), position, 0)
end

local combat = Combat()
combat:setArea(createCombatArea(AREA_CIRCLE3X3))
combat:setCallback(CALLBACK_PARAM_TARGETTILE, "onTargetTile")

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("Trial Frost Spell")
spell:words("frigo")
spell:level(1)
spell:mana(0)
spell:group("attack")
spell:isAggressive(true)
spell:cooldown(0)
spell:groupCooldown(0)
spell:needLearn(false)
spell:vocation("sorcerer", "master sorcerer")
spell:register()
