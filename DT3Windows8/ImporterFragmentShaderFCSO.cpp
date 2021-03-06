//==============================================================================
///	
///	File: ImporterFragmentShaderFCSO.cpp
///	
/// Copyright (C) 2000-2013 by Smells Like Donkey, Inc. All rights reserved.
///
/// This file is subject to the terms and conditions defined in
/// file 'LICENSE.txt', which is part of this source code package.
///	
//==============================================================================

#include "ImporterFragmentShaderFCSO.hpp"
#include "Factory.hpp"
#include "FragmentShaderResource.hpp"
#include "MaterialResource.hpp"
#include "FilePath.hpp"
#include "System.hpp"
#include "DeviceConsole.hpp"
#include "ResourceManager.hpp"
#include "ImporterFragmentShader.hpp"
#include "BinaryFileStream.hpp"
#include "DeviceFileManager.hpp"

//==============================================================================
//==============================================================================
 
namespace DT2 {

//==============================================================================
/// Register with object factory
//==============================================================================

IMPLEMENT_FACTORY_IMPORTER(ImporterFragmentShaderFCSO,fcso)

//==============================================================================
/// Standard class constructors/destructors
//==============================================================================

ImporterFragmentShaderFCSO::ImporterFragmentShaderFCSO (void)
{    
	
}
			
ImporterFragmentShaderFCSO::~ImporterFragmentShaderFCSO (void)
{ 

}

//==============================================================================
//==============================================================================

void ImporterFragmentShaderFCSO::parseResourceBlock (FragmentShaderResource *target)
{
	_tokenizer.assumeNextToken("=");

	String resource_name = _tokenizer.getNextTokenString();
    DTuint resource_index = _tokenizer.getNextTokenNumber();

    target->setShaderResourceIndex(resource_name, resource_index);
}

//==============================================================================
//==============================================================================

void ImporterFragmentShaderFCSO::parseResourcesBlock (FragmentShaderResource *target)
{
	_tokenizer.assumeNextToken("{");

	while (true) {
		String token = _tokenizer.getNextTokenString();
	
		// Handle Preprocessor
		if (_tokenizer.parsePreprocessorMacros(token))
			continue;
			
		// Are we at the end of the block
		if (token == "}")	break;
		
		if (Tokenizer::matchToken(token,"resource"))			{	parseResourceBlock(target);	    	continue;	}
	};
	
}

//==============================================================================
//==============================================================================

void ImporterFragmentShaderFCSO::parseShaderBlock (FragmentShaderResource *target)
{
	_tokenizer.assumeNextToken("=");

	String shader_hex = _tokenizer.getNextTokenString();	
    String shader_raw;

    shader_raw.resize(shader_hex.size() / 2, 0);

    MoreMath::fromHexString (shader_hex, &shader_raw[0], shader_raw.size());

    target->setShader("HLSL",shader_raw);
}

//==============================================================================
//==============================================================================

void ImporterFragmentShaderFCSO::parseProgramBlock (FragmentShaderResource *target)
{
	_tokenizer.assumeNextToken("{");

	while (true) {
		String token = _tokenizer.getNextTokenString();
	
		// Handle Preprocessor
		if (_tokenizer.parsePreprocessorMacros(token))
			continue;
			
		// Are we at the end of the block
		if (token == "}")	break;
		
		if (Tokenizer::matchToken(token,"resources"))			{	parseResourcesBlock(target);	    	continue;	}
		if (Tokenizer::matchToken(token,"shader"))			    {	parseShaderBlock(target);	        	continue;	}
	};
	
}

//==============================================================================
//==============================================================================

DTerr ImporterFragmentShaderFCSO::import(FragmentShaderResource *target, String args)
{
	DTerr err;
	if ((err = _tokenizer.loadTokenStream (target->getPath())) != ERR_NONE)
		return ERR_FILE_OPEN_FAILED;
	
	while (!_tokenizer.isDone()) {
		String token = _tokenizer.getNextTokenString();
		
		// Handle Preprocessor
		if (_tokenizer.parsePreprocessorMacros(token))
			continue;
		
		if (Tokenizer::matchToken(token,"shader"))		{	parseProgramBlock(target);		continue;	}
		
	};
	
    target->addDependencies(_tokenizer.getDependencies());

	return ERR_NONE;
}

//==============================================================================
//==============================================================================

} // DT2

