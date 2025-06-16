-- Простой скрипт: вращает объект вокруг оси Y каждый кадр
-- Ожидается, что в Lua доступна глобальная переменная dt (дельта-время в секундах),
-- а также функции get_position(), set_position(x,y,z), rotate(axisTable, angle).

function update()
    -- Можно прочитать текущую позицию, если нужно
    -- local pos = get_position()
    -- print(string.format("Lua: current pos = (%.2f, %.2f, %.2f)", pos[1], pos[2], pos[3]))

    -- Вращаем вокруг Y с угловой скоростью 45 градусов/с
    local angle = dt * 45.0
    rotate({0, 1, 0}, angle)
end
