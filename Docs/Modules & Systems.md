# Modules & Systems

Tree Engine's structure is inspired by Source engine's modularity. This was not necessary, but rather done as a technical challenge of sorts.

Making a C++ program modular is not a trivial task, as C++ has no reflection. Tree Engine thus contains some scaffolding to help with this.

### Terminology

- ***Module***: Inspired by Win32 terminology, this refers to a file that contains code.
    - On Windows: `.exe` executable or `.dll` dynamic library
    - On Linux: Executable or `.so` shared library
- ***SysGroup***: An interface for accessing a *Module*'s *System*s, and setting which *System*s it can access.
- ***System***: Represents some kind of facility within a *Module*. For example, a windowing system, or a render backend.

## Tree Engine Structure

Tree Engine has a number of modules and systems:

- Entry points:
    - `Tree.Launcher.Game` module: Normal instance of the engine, to be shipped to the average user. May run a listen server.
    - `Tree.Launcher.Editor` module: Special developer-focused variant of Game with extra UI to aid in development.
    - `Tree.Launcher.DedicatedServer` module: Dedicated server / headless instance. Skips windowing, rendering, audio.
        - All contain the `SysGroupManager` system, providing facilities to all modules for loading/unloading modules and their systems.
- System modules:
    - `Tree.Root` module
        - `EngineSystem` system: Main loop and other top-level engine services
        - `LogSystem` system: Logging facilities
        - `CmdLineSystem` system: Captures arguments and flags from the command line
        - `ManagedHostSystem` system: Hosts managed (C#) code. Responsible for C++ ↔ C# interop (Sap).
            - Calls from C# into other modules pass through `Tree.Root` as a sort of chokepoint as a consequence.
    - `Tree.Window` module
        - `WindowSystem` system: Windowing facilities. Captures input and displays rendered frames.
    
... and likely more, as this document probably isn't fully up-to-date.

### Behavior

Ultimately, modules access all globally accessible systems through the static [`Sys` struct](/Source/Tree.NativeCommon/sys.h). This contains getters that return pointers to those systems, implemented as virtual classes. Virtual method tables are leveraged to circumvent the need for linking modules at compile-time.

To load those systems, we have [`SysGroupManager`](/Source/Tree.Launcher/sysgroupmanager.h). The manager is itself a system, to make it accessible from all modules. If system modules have their own dependencies and wish to load them (for example, if `Tree.Render` wishes to load a DX12 backend from `Tree.Render.DX12`) they may do so through this system.

`SysGroupManager` loads systems on program entry as follows:
1. The manager is first bootstrapped. This makes its global instance accessible through `Sys` in the `Tree.Launcher` module. We need to do this because the manager is a system, but nothing else exists to set `Sys`'s getters, and the system registry is too cumbersome to interface with directly. 
2. `SysGroupManager::LoadGroupsFrom` is called from the entrypoint, with a list of modules appropriate for the current domain (Game/Editor/DedicatedServer).
3. The manager loads all of the modules into memory and creates `SysGroup`s for them, storing them all in a list. The `SysGroup` containing the manager is already present from the previous bootstrapping.
4. The manager loops through all of the stored `SysGroup`s, and informs every `SysGroup` of every other `SysGroup`.

Every relevant system is now available in every loaded module, including `Tree.Launcher`. The engine is then initialized.

### Limitations

This approach is necessary due to the separation of object memory between modules. The `Sys` struct's pointers must be assigned in every module or you risk accessing a null pointer.

The separation of memory also has an unfortunate side effect: Memory allocated in one module cannot be deallocated in another module, which may happen when using standard library shared pointers. This necessitates the use of a central memory de/allocation facility in Tree Engine, which has not been written as of the writing of this document.
