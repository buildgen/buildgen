-- Copyright 2011-2012 Kevin Cox

--[[---------------------------------------------------------------------------]
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

require "lfs"
require "pl"
stringx.import()

flags = {
	v = false, -- be verbose.
}

local i = 1; -- Even though lua indicies start at 1 the script name is still 0.
while i <= #arg do
	if arg[i]:find("-", 1, true) == 1 then
		flags[arg[i]:sub(2)] = true
	else
		break
	end

	i = i+1
end

while i < #arg do
	if flags.v then print("Moving "..arg[i].."...") end
	dir.movefile(arg[i], arg[#arg]);

	i = i+1
end
