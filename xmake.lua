add_rules("mode.debug", "mode.release")

add_rules("plugin.compile_commands.autoupdate", {
  outputdir = "./"
})

target("loulang")
  set_kind("binary")
  add_includedirs("src")
  add_files("src/*.c", "src/*/*.c")
  set_warnings("all", "error")
  if is_mode("debug") then
    set_policy("build.sanitizer.address", true)
  end

