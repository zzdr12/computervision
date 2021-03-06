add_syslinks("pthread","gxiapi","cjson")
add_requires("opencv")

-- includes("Camera")
-- includes("Controller")
-- includes("Detector")
-- includes("PictureManipulator")
-- includes("SerialPort")
-- includes("Share")
-- includes("Socket")


target("CV")
    set_kind("hero")
    set_rules("mode.debug", "mode.release")
    set_optimize("fastest")
    -- on_load(function (target)
    --     import("lib.detect.find_package")
    --     target:add(find_package("opencv"))
    --     end)
    add_includedirs("include")
    add_files("src/*.cpp")
    add_packages("opencv")
