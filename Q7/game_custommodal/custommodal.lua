modalWindow = nil
jumpButton = nil
modalSize = nil
posPadding = {}

function init()
    g_keyboard.bindKeyDown('Ctrl+U', toggle)
    modalWindow = g_ui.displayUI('custommodal')
    modalWindow:setVisible(false)

    jumpButton = modalWindow:getChildById('jumpButton')

    connect(g_game, {
        onGameStart = online,
        onGameEnd = offline
    })

    --Init modal Size here since it won't change
    modalSize = modalWindow:getSize()
    --Calculate padding since it depends on modalSize and won't change either
    posPadding = { x = modalSize.width * 0.1, y = modalSize.height * 0.1 }
end

function terminate()
  disconnect(g_game, {
    onGameStart = online,
    onGameEnd = offline
  })

  g_keyboard.unbindKeyDown('Ctrl+U')

end

function online()
  hide()
end

function offline()
  hide()
end

function show()
  if not g_game.isOnline() then
    return
  end
  modalWindow:show()
  modalWindow:raise()
  modalWindow:focus()

  --When showing customModal, jump the button and start its Tick function
  jump()
  buttonTick()
end

function hide()
  modalWindow:hide()
end

function toggle()
  if not modalWindow:isVisible() then
    show()
  else
    hide()
  end
end

function cancel()
  hide()
end

modalPosition = nil
modalBounds = {}

--Update modal info to account for dragging the modal around
function updateModalInfo()
    modalPosition = modalWindow:getPosition()
    modalBounds = 
    {
        min = { x = modalPosition.x, y = modalPosition.y },
        max = { x = modalPosition.x + modalSize.width, y = modalPosition.y + modalSize.height}
    }
end

--determine if jumpButton is outside of the modal's bounds
function isOutOfBounds()
    local pos = jumpButton:getPosition()
    if pos.x < modalBounds.min.x or pos.x > modalBounds.max.x then
        return true
    elseif pos.y < modalBounds.min.y or pos.y > modalBounds.max.y then
        return true
    end
    return false
end

--jump the button to a random position within the modal
function jump()
    updateModalInfo()

    local newPos = 
    {
        x = math.random(modalBounds.min.x + modalSize.width * 0.75, modalBounds.max.x - posPadding.x), 
        y = math.random(modalBounds.min.y + posPadding.y, modalBounds.max.y - posPadding.y)
    }

    jumpButton:setPosition(newPos)
end

BUTTON_TICK_DURATION = 10
BUTTON_SPEED = 200 * BUTTON_TICK_DURATION * 0.001

--scroll the button from right to left
function scroll()
    updateModalInfo()

    jumpButton:move(jumpButton:getX() - BUTTON_SPEED, jumpButton:getY())

    if isOutOfBounds() then
        jump()
    end
end

function buttonTick()
    removeEvent(jumpButton.waitEvent)

    --stop ticking if the modal is no longer visible
    if not modalWindow:isVisible() then
        return
    end

    scroll()

    jumpButton.waitEvent = scheduleEvent(function()
        buttonTick()
    end, BUTTON_TICK_DURATION)
end