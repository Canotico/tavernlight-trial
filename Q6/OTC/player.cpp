/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "player.h"

void Player::turn(Otc::Direction direction)
{
    Creature::turn(direction);
    //OnTurn set the amount of frames we'll want the dash effect to be visible
    dashFramesRemaining = 300;
}

void Player::internalDrawOutfit(Point dest, float scaleFactor, bool animateWalk, bool animateIdle, Otc::Direction direction, LightView* lightView)
{
    g_painter->setColor(m_outfitColor);

    // outfit is a real creature
    if (m_outfit.getCategory() == ThingCategoryCreature) {
        int animationPhase = animateWalk ? m_walkAnimationPhase : 0;

        if (isAnimateAlways() && animateIdle) {
            int ticksPerFrame = 1000 / getAnimationPhases();
            animationPhase = (g_clock.millis() % (ticksPerFrame * getAnimationPhases())) / ticksPerFrame;
        }

        // xPattern => creature direction
        int xPattern;
        if (direction == Otc::NorthEast || direction == Otc::SouthEast)
            xPattern = Otc::East;
        else if (direction == Otc::NorthWest || direction == Otc::SouthWest)
            xPattern = Otc::West;
        else
            xPattern = direction;

        int zPattern = 0;
        if (m_outfit.getMount() != 0) {
            auto datType = g_things.rawGetThingType(m_outfit.getMount(), ThingCategoryCreature);
            dest -= datType->getDisplacement() * scaleFactor;
            datType->draw(dest, scaleFactor, 0, xPattern, 0, 0, animationPhase, lightView);
            dest += getDisplacement() * scaleFactor;
            zPattern = std::min<int>(1, getNumPatternZ() - 1);
        }

        PointF jumpOffset = m_jumpOffset * scaleFactor;
        dest -= Point(stdext::round(jumpOffset.x), stdext::round(jumpOffset.y));

        // yPattern => creature addon
        for (int yPattern = 0; yPattern < getNumPatternY(); yPattern++) {

            // continue if we dont have this addon
            if (yPattern > 0 && !(m_outfit.getAddons() & (1 << (yPattern - 1))))
                continue;

            auto datType = rawGetThingType();
            datType->draw(dest, scaleFactor, 0, xPattern, yPattern, zPattern, animationPhase, yPattern == 0 ? lightView : nullptr);

            if (getLayers() > 1) {
                Color oldColor = g_painter->getColor();
                Painter::CompositionMode oldComposition = g_painter->getCompositionMode();

                //Default
                g_painter->setCompositionMode(Painter::CompositionMode_Multiply);
                g_painter->setColor(m_outfit.getHeadColor());
                datType->draw(dest, scaleFactor, SpriteMaskYellow, xPattern, yPattern, zPattern, animationPhase);
                g_painter->setColor(m_outfit.getBodyColor());
                datType->draw(dest, scaleFactor, SpriteMaskRed, xPattern, yPattern, zPattern, animationPhase);
                g_painter->setColor(m_outfit.getLegsColor());
                datType->draw(dest, scaleFactor, SpriteMaskGreen, xPattern, yPattern, zPattern, animationPhase);
                g_painter->setColor(m_outfit.getFeetColor());
                datType->draw(dest, scaleFactor, SpriteMaskBlue, xPattern, yPattern, zPattern, animationPhase);
                g_painter->setColor(oldColor);
                g_painter->setCompositionMode(oldComposition);
            }

            /*
            Dash Afterimages
            - Copy the set of instructions from above to draw clones of the character
            */
            if (dashFramesRemaining > 0)
            {
                for (size_t i = 0; i < 4; i++)
                {
                    Point offset = Point(8 * i, 8 * i);
                    dest += offset;

                    datType->draw(dest, scaleFactor, 0, xPattern, yPattern, zPattern, animationPhase, yPattern == 0 ? lightView : nullptr);

                    if (getLayers() > 1) {
                        Color oldColor = g_painter->getColor();
                        Painter::CompositionMode oldComposition = g_painter->getCompositionMode();

                        //Default
                        g_painter->setCompositionMode(Painter::CompositionMode_Multiply);
                        g_painter->setColor(m_outfit.getHeadColor());
                        datType->draw(dest, scaleFactor, SpriteMaskYellow, xPattern, yPattern, zPattern, animationPhase);
                        g_painter->setColor(m_outfit.getBodyColor());
                        datType->draw(dest, scaleFactor, SpriteMaskRed, xPattern, yPattern, zPattern, animationPhase);
                        g_painter->setColor(m_outfit.getLegsColor());
                        datType->draw(dest, scaleFactor, SpriteMaskGreen, xPattern, yPattern, zPattern, animationPhase);
                        g_painter->setColor(m_outfit.getFeetColor());
                        datType->draw(dest, scaleFactor, SpriteMaskBlue, xPattern, yPattern, zPattern, animationPhase);
                        g_painter->setColor(oldColor);
                        g_painter->setCompositionMode(oldComposition);
                    }
                }
                //Count down the amount of frames remaining for this effect to be visible
                dashFramesRemaining--;
            }
        }
        // outfit is a creature imitating an item or the invisible effect
    }
    else {
        ThingType* type = g_things.rawGetThingType(m_outfit.getAuxId(), m_outfit.getCategory());

        int animationPhase = 0;
        int animationPhases = type->getAnimationPhases();
        int animateTicks = Otc::ITEM_TICKS_PER_FRAME;

        // when creature is an effect we cant render the first and last animation phase,
        // instead we should loop in the phases between
        if (m_outfit.getCategory() == ThingCategoryEffect) {
            animationPhases = std::max<int>(1, animationPhases - 2);
            animateTicks = Otc::INVISIBLE_TICKS_PER_FRAME;
        }

        if (animationPhases > 1) {
            if (animateIdle)
                animationPhase = (g_clock.millis() % (animateTicks * animationPhases)) / animateTicks;
            else
                animationPhase = animationPhases - 1;
        }

        if (m_outfit.getCategory() == ThingCategoryEffect)
            animationPhase = std::min<int>(animationPhase + 1, animationPhases);

        type->draw(dest - (getDisplacement() * scaleFactor), scaleFactor, 0, 0, 0, 0, animationPhase, lightView);
    }

    g_painter->resetColor();
}
