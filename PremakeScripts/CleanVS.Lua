newaction {
    trigger     = "cleanvs",
    description = "remove Visual Studio files",
    execute     = function ()
        print("clean the build...")
        os.rmdir("./build")
        print("done.")
    end
}