add_rules("mode.debug", "mode.release")

target("louParser")
  set_kind("shared")
  add_includedirs("include", { public = true })
  add_includedirs("src")
  add_deps("louCore", "louLexer")
  add_files("src/*.c", "src/*/*.c")
  project_config() 

