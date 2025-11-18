add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "./" })

includes("logger")
includes("lexer")
includes("parser")

target("loulang")
  set_kind("binary")
  set_languages("c++20")
  add_deps("logger", "lexer", "parser")
  add_includedirs("src")
  add_files("src/*.cpp")

