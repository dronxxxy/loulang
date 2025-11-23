add_rules("mode.debug", "mode.release")

target("louLexer")
  set_kind("shared")
  add_includedirs("include", { public = true })
  add_includedirs("src")
  add_deps("louCore")
  add_files("src/*.c")

