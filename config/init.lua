-- this is an example init file for the editor config
-- not reflective of the final API usage

local function foo()
    print('bar')
end

-- the command registry for your eh-ditor
-- you can:
--   1. add commands (must be prefixed with '/' when used in the console)
--   2. delete commands
eh.commands.add('file.save',   foo)
eh.commands.add('file.delete', foo)
eh.commands.add('file.copy',   foo)
eh.commands.del('file.save')
eh.commands.del('file.delete')
eh.commands.del('file.copy')

-- the keymap for your eh-ditor
-- you can:
--   1. set keybinds which invoke lua callbacks
--   2. delete keybinds
eh.keymap.set('<ctrl> ~', eh.console.toggle)
eh.keymap.set('<ctrl> c', 'text.copy')  -- e.g. copy highlighted text
eh.keymap.set('<ctrl> z', 'state.undo') -- e.g. undo current state
eh.keymap.del('<ctrl> ~')
eh.keymap.del('<ctrl> c')
eh.keymap.del('<ctrl> z')

-- the console for your eh-ditor
-- you can:
--   1. toggle it (ON->OFF or OFF->ON)
--   2. explicitly enable or disable it
--   3. set the font/color/shader
eh.console.toggle()
eh.console.enabled = false
eh.console.font    = 'JetBrains Mono'
eh.console.color   = 0xFFFFFF         -- implicitly applies a simple shader to display said color
eh.console.shader  = 'my-shader.glsl' -- probes ~/.config/eh/shaders/ for the desired shader

-- the editor for your eh-ditor
-- you can:
--   1. get/set the font
--   2. get/set the cursor position
eh.editor.font     = 'JetBrains Mono'
eh.editor.cursor   = {x = 0, y = 0}
eh.editor.cursor.x = 1
eh.editor.cursor.y = 1
