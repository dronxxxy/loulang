add_rules("mode.debug", "mode.release")

target("louCore")
  set_kind("shared")
  add_includedirs("include", { public = true })
  add_includedirs("src")
  add_files("src/*.c")
  project_config() 

