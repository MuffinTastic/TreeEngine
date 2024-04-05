#include "cmdlinesystem.h"

#include <optional>
#include <tuple>
#include <sstream>
#include <charconv>

#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/sys.h"


REGISTER_TREE_SYSTEM( CmdLineSystem, CMDLINESYSTEM_NAME )


Tree::ESystemInitCode Tree::CmdLineSystem::Startup()
{
    return ESYSTEMINIT_SUCCESS;
}

void Tree::CmdLineSystem::Shutdown()
{

}

void Tree::CmdLineSystem::ProcessArguments( std::vector<std::string> arguments )
{
	m_arguments = arguments;

	// name, isCommand
	std::optional<std::tuple<std::string, bool>> prev = {};

	for ( auto it = m_arguments.begin(); it != m_arguments.end(); ++it )
	{
		std::string current = *it;
		if ( current.size() == 0 )
			continue;

		char firstChar = current.at( 0 );

		if ( firstChar == '-' || firstChar == '+' )
		{
			prev = {};

			std::string name( current.begin() + 1, current.end() );
			if ( name.size() == 0 )
				continue;

			bool isCommand = firstChar == '+';

			auto nextIt = ( it + 1 );
			bool nextIsEnd = nextIt == m_arguments.end();
			bool nextHasSpecial = false;

			if ( !nextIsEnd )
			{
				std::string_view next = *nextIt;
				char nextFirstChar = next.at( 0 );
				nextHasSpecial = nextFirstChar == '-' || nextFirstChar == '+';
			}

			bool single = nextIsEnd || nextHasSpecial;

			if ( !isCommand )
				m_flags.insert( name );

			if ( single )
			{
				if ( isCommand )
					InsertCommand( name, {} );
			}
			else
			{
				prev = std::make_tuple( std::move( name ), isCommand );
			}

		}
		else if ( prev.has_value() )
		{
			std::string name = std::get<0>( *prev );
			bool isCommand = std::get<1>( *prev );

			if ( isCommand )
				InsertCommand( name, current );
			else
				m_options.insert( { name, current } );

			prev = {};
		}
	}
}

std::vector<std::string> Tree::CmdLineSystem::GetArguments() const
{
	return m_arguments;
}

bool Tree::CmdLineSystem::GetFlag( std::string name ) const
{
	return m_flags.find( name ) != m_flags.end();
}

std::string Tree::CmdLineSystem::GetStringOption( std::string name, std::string def ) const
{
	auto it = m_options.find( name );
	if ( it == m_options.end() )
		return def;

	return it->second;
}

int Tree::CmdLineSystem::GetIntOption( std::string name, int def ) const
{
	auto it = m_options.find( name );
	if ( it == m_options.end() )
		return def;

	std::string_view optionValue = it->second;
	int returnValue = def;

	auto result = std::from_chars( optionValue.data(), optionValue.data() + optionValue.size(), returnValue);

	if ( result.ec == std::errc::invalid_argument )
	{
		Platform::DebugLog( "Invalid format while parsing option '{}', using default", name );
		return def;
	}
	else if ( result.ec == std::errc::result_out_of_range )
	{
		Platform::DebugLog( "Value of option '{}' was too big for int range, using default", name );
		return def;
	}

	return returnValue;
}

std::vector<std::string> Tree::CmdLineSystem::GetCommands() const
{
	return m_commands;
}

void Tree::CmdLineSystem::InsertCommand( std::string_view first, std::optional<std::string_view> second )
{
	std::stringstream ss;
	
	ss << "\"" << first << "\"";
	
	if ( second.has_value() )
	{
		ss << " \"" << *second << "\"";
	}

	m_commands.push_back( std::move( ss.str() ) );
}
