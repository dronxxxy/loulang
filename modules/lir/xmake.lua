includes("modules/x86", "modules/elf")

project_module("louLir", { "louCore", "lirX86", "lirElf" })

target("lirCodegenTests")
  set_kind("binary")
  set_default(false)
  add_files("tests/codegen/**.c")
  add_deps("lirX86", "lirElf")
  on_test(function (target, opt)
    local output = "$(builddir)/" .. opt.name

    local dir = path.directory(output)
    if not os.exists(dir) then
      os.mkdir(dir)
    end

    local file = io.open(output, "w", { encoding = "binary" })
    os.runv(target:targetfile(), { opt.name }, { stdout = file })
    output = file:path()
    file:close()

    os.runv("chmod", { "+x", path.absolute(output) });

    import("core.base.pipe")
    import("core.base.bytes")

    local rpipe, wpipe = pipe.openpair()
    os.runv(output, { opt.name }, { stdout = wpipe });
    wpipe:close()

    local buff = bytes(8192)
    local read, stdout = rpipe:read(buff, 8192)
    local result;
    if stdout == nil then
      result = ""
    else 
      result = stdout:str()
    end

    if opt.stdout ~= nil and opt.stdout ~= result then
      print("expected:\n" .. opt.stdout .. "\ngot:\n" .. result)
      return false
    end

    return true
  end)
  add_tests("hello-world", { stdout = "Hello, world!\n" })
  project_config()


