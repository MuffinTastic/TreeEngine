﻿namespace Tree.Engine.Hotload;

/// <summary>
/// Represents the required information for a project assembly.
/// </summary>
public readonly struct ProjectAssemblyInfo
{
    /// <summary>
    /// The name of the projects assembly.
    /// </summary>
    public string AssemblyName { get; init; }

    /// <summary>
    /// The relative path to the project.
    /// </summary>
    public string SourceRoot { get; init; }

    /// <summary>
    /// The relative path to the projects csproj file.
    /// </summary>
    public string ProjectPath { get; init; }

    /// <summary>
    /// Are we building this assembly for the server?
    /// If not, we can safely assume that it's for the client.
    /// </summary>
    public bool IsServer { get; init; }
}
