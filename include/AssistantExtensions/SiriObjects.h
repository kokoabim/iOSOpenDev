//
//  SiriObjects.h
//  AssistantExtensions
//	Version 1.0.1
//
//  Created by K3A.
//  Copyright (c) 2012 K3A.me. All rights reserved.
//

#pragma once
#include <Foundation/Foundation.h>

#ifdef SC_PRIVATE
# import "SiriObjects_private.h"
#endif

/// Ordinary assistant object dictionary (NSMutableDictionary) containing class name, group and properties.
typedef NSMutableDictionary SOObject;
/// Root assistant object (NSMutableDictionary). Like SOObject but with aceId and refId properties.
typedef SOObject SOAceObject; 

@protocol SECommand;


/// Protocol specifying methods of context passed via handleSpeech to the class conforming to SECommand
@protocol SEContext <NSObject>
@required
/** Creates a dictionary representing an ordinary assistant object
 \param className Name of the class
 \param group Name of the object group
 \param props Properties of the object
 */
-(SOObject*)createObjectDict:(NSString*)className group:(NSString*)group properties:(NSDictionary*)props;

/** Creates a dictionary representing utterance view
 \param text A text to show and speak
 */
-(SOObject*)createAssistantUtteranceView:(NSString*)text;
/** Creates a dictionary representing utterance view
 \param text A text to show
 \param speakableText A text to speak
 */
-(SOObject*)createAssistantUtteranceView:(NSString*)text speakableText:(NSString*)speakableText;
/** Creates a dictionary representing utterance view
 \param text A text to show
 \param speakableText A text to speak
 \param dialogIdentifier Dialog identifier
 */
-(SOObject*)createAssistantUtteranceView:(NSString*)text speakableText:(NSString*)speakableText dialogIdentifier:(NSString*)dialogIdentifier;
/** Creates a dictionary representing a snippet
 \param snippetClass Name of the snippet class
 \param props Dictionary of snippet properties
 */
-(SOObject*)createSnippet:(NSString*)snippetClass properties:(NSDictionary*)props;

/** Sends a root assistant object 
 */
-(BOOL)sendAceObject:(NSString*)className group:(NSString*)group properties:(NSDictionary*)props;

/** Sends "request completed" to the assistant
 */
-(BOOL)sendRequestCompleted;

/** Sends views to the assistant
 \param views Array of views to send
 */
-(BOOL)sendAddViews:(NSArray*)views;
/** Sends views to the assistant
 \param views Array of views to send
 \param dialogPhase Dialog phase, e.g. Reflection (will be replaced), Completion, Summary, Clarification, Error, Acknowledgement
 \param scrollToTop Should scroll to top
 \param temporary Temporary flag
 */
-(BOOL)sendAddViews:(NSArray*)views dialogPhase:(NSString*)dialogPhase scrollToTop:(BOOL)scrollToTop temporary:(BOOL)temporary;

/** Create a snippet and send it immediately
 \param snippetClass Name of the snippet class
 \param props Properties for the snippet class
 */
-(BOOL)sendAddViewsSnippet:(NSString*)snippetClass properties:(NSDictionary*)props;
/** Create a snippet and send it immediately
 \param snippetClass Name of the snippet class
 \param properties Properties for the snippet class
 \param dialogPhase Dialog phase, e.g. Reflection (will be replaced), Completion, Summary, Clarification, Error, Acknowledgement
 \param scrollToTop Should scroll to top
 \param temporary Temporary flag
 */
-(BOOL)sendAddViewsSnippet:(NSString*)snippetClass properties:(NSDictionary*)props dialogPhase:(NSString*)dialogPhase scrollToTop:(BOOL)scrollToTop temporary:(BOOL)temporary;
  
/** Create utterance (text) and send it immediately
 \param text Text to send
 */
-(BOOL)sendAddViewsUtteranceView:(NSString*)text;
/** Create utterance (text) and send it immediately
 \param text Text to send
 \param speakableText Text to speak (different from what is displayed)
 */
-(BOOL)sendAddViewsUtteranceView:(NSString*)text speakableText:(NSString*)speakableText;
/** Create utterance (text) and send it immediately
 \param text Text to send
 \param speakableText Text to speak (different from what is displayed)
 \param dialogPhase Dialog phase, e.g. Reflection (will be replaced), Completion, Summary, Clarification, Error, Acknowledgement
 */
-(BOOL)sendAddViewsUtteranceView:(NSString*)text speakableText:(NSString*)speakableText dialogPhase:(NSString*)dialogPhase scrollToTop:(BOOL)scrollToTop temporary:(BOOL)temporary;
/** Create utterance (text) and send it immediately
 \param text Text to send
 \param speakableText Text to speak (different from what is displayed)
 \param dialogPhase Dialog phase, e.g. Reflection (will be replaced), Completion, Summary, Clarification, Error, Acknowledgement
 \param listen Whether to listen for user response just after speaking the text. If YES, next request will be sent to the same SECommand.
 \since 1.0.1
 */
-(BOOL)sendAddViewsUtteranceView:(NSString*)text speakableText:(NSString*)speakableText dialogPhase:(NSString*)dialogPhase scrollToTop:(BOOL)scrollToTop temporary:(BOOL)temporary listenAfterSpeaking:(BOOL)listen; //SINCE 1.0.1

/** Returns a boolean indicating whether the current request has been completed (by calling sendRequestCompleted). */
-(BOOL)requestHasCompleted;

/** Returns an object which handled the request or nil if the request has not been handled yet. */
-(NSObject<SECommand>*)object;

/** Returns a string with refId of the current request. */
-(NSString*)refId;

@end

/** Protocol specifying methods of an extension class representing snippet.
    Instances of these classes will be created for every snippet creation request.
    A class representing snippet can derive from UIView or just represent some proxy class creating it's UIView on demand.
    Don't forget you really should prefix your class with some shortcut, e.g. K3AAwesomeSnippet!
 */
@protocol SESnippet <NSObject>
@required
/// Initializes a snippet by properties 
-(id)initWithProperties:(NSDictionary*)props;
/// Returns a view representing snippet, can be self if the conforming class is already UIView
-(id)view;

@end

/** Protocol specifying methods of an extension class handling commands.
    Classes conforming to this protocol are initialized just after loading bundle and will remain in memory.
    Don't forget you really should prefix your class with some shortcut, e.g. K3AAwesomeCommand!
 */
@protocol SECommand <NSObject>
@required
/** Allows the extension to react to recognized text.
    \param text Recognized text
    \param tokens An ordered array of lowercase tokens representing recognized words
    \param tokenset An unordered set of tokens for fast search - recommended parameter for keyword-matching
    \param ctx Context representing the current request also allowing to send responses
 */
-(BOOL)handleSpeech:(NSString*)text tokens:(NSArray*)tokens tokenSet:(NSSet*)tokenset context:(id<SEContext>)ctx;

@end


/** Protocol specifying methods for the NSPrincipal class of the extension.
 */
@protocol SESystem <NSObject>
@required
/// Register a command class
-(BOOL)registerCommand:(Class)cls;
/// Register a snippet class
-(BOOL)registerSnippet:(Class)cls;
/// Returns the version string of AssistantExtensions
-(NSString*)systemVersion; //SINCE 1.0.1
@end

/// Protocol specifying methods of the extension's principal class
@protocol SEExtension <NSObject>

@required

/// The first method which is called on your class, system is where you register commands and snippets
-(id)initWithSystem:(id<SESystem>)system;

@optional

/// Extension author
-(NSString*)author;
/// Extension name
-(NSString*)name;
/// Extension description
-(NSString*)description;
/// Extension website URL
-(NSString*)website;
/// Minimal SiriExtensions version requirement
-(NSString*)versionRequirement; //SINCE 1.0.1

@end