--- The Lua Library
-- @module S.lua

-- Copyright 2011-2012 Kevin Cox

--[[ --------------------------------------------------------------------------]
[                                                                              ]
[  This software is provided 'as-is', without any express or implied           ]
[  warranty. In no event will the authors be held liable for any damages       ]
[  arising from the use of this software.                                      ]
[                                                                              ]
[  Permission is granted to anyone to use this software for any purpose,       ]
[  including commercial applications, and to alter it and redistribute it      ]
[  freely, subject to the following restrictions:                              ]
[                                                                              ]
[  1. The origin of this software must not be misrepresented; you must not     ]
[     claim that you wrote the original software. If you use this software in  ]
[     a product, an acknowledgment in the product documentation would be       ]
[     appreciated but is not required.                                         ]
[                                                                              ]
[  2. Altered source versions must be plainly marked as such, and must not be  ]
[     misrepresented as being the original software.                           ]
[                                                                              ]
[  3. This notice may not be removed or altered from any source distribution.  ]                                                           *
[                                                                              ]
[-----------------------------------------------------------------------------]]

S.lua = {}

--- Compile a Script into Lua Bytecode.
-- Pre-compiling a Lua script has a number of advantages such as decreased load
-- time (usually the script has to be compiled before each run), smaller files
-- and possibly obfuscation.
--
-- @tparam string source The input file.
-- @tparam string out Where to place the bytecode.
function S.lua.compile ( source, out )
	T.utils.assert_string(1, source)
	T.utils.assert_string(2, out)

	source = C.path(source)
	out    = C.path(out)

	C.addGenerator({source}, {"*luac", "-o", out, source}, {out})

	return out
end
