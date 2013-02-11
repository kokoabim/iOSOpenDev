#import <UIKit/UIKit.h>
#import <libkern/OSAtomic.h>

// libactivator
// Centralized gestures and button management for iPhone OS

// Events

@interface LAEvent : NSObject<NSCoding> {
@private
	NSString *_name;
	NSString *_mode;
	BOOL _handled;
}
+ (id)eventWithName:(NSString *)name;
+ (id)eventWithName:(NSString *)name mode:(NSString *)mode;
- (id)initWithName:(NSString *)name;
- (id)initWithName:(NSString *)name mode:(NSString *)mode;

@property (nonatomic, readonly) NSString *name;
@property (nonatomic, readonly) NSString *mode;
@property (nonatomic, getter=isHandled) BOOL handled;

@end

@protocol LAEventDataSource <NSObject>

@required
- (NSString *)localizedTitleForEventName:(NSString *)eventName;
- (NSString *)localizedGroupForEventName:(NSString *)eventName;
- (NSString *)localizedDescriptionForEventName:(NSString *)eventName;

@optional
- (BOOL)eventWithNameIsHidden:(NSString *)eventName;
- (BOOL)eventWithName:(NSString *)eventName isCompatibleWithMode:(NSString *)eventMode;

@end

typedef enum {
    LAActivatorVersion_1_3 = 1030000,
    LAActivatorVersion_1_4 = 1040000,
    LAActivatorVersion_1_5 = 1050000,
    LAActivatorVersion_1_5_1 = 1050100,
    LAActivatorVersion_1_5_2 = 1050200,
    LAActivatorVersion_1_5_3 = 1050300,
    LAActivatorVersion_1_5_4 = 1050400,
    LAActivatorVersion_1_5_5 = 1050500,
    LAActivatorVersion_1_5_7 = 1050700,
    LAActivatorVersion_1_5_8 = 1050800,
    LAActivatorVersion_1_5_9 = 1050900,
    LAActivatorVersion_1_6 = 1060000,
    LAActivatorVersion_1_6_1 = 1060100,
    LAActivatorVersion_1_6_2 = 1060200,
    LAActivatorVersion_1_7 = 1070000,
    LAActivatorVersion_1_7_1 = 1070100,
    LAActivatorVersion_1_7_2 = 1070200,
    LAActivatorVersion_1_7_3 = 1070300,
    LAActivatorVersion_1_7_4 = 1070400,
} LAActivatorVersion;

// Activator

@protocol LAListener;

@interface LAActivator : NSObject {
@private
	NSArray *_availableEventModes;
@protected
    NSDictionary *_cachedPreferences;
	NSMutableDictionary *_cachedListenerTitles;
	NSMutableDictionary *_cachedListenerDescriptions;
	NSMutableDictionary *_cachedListenerGroups;
	NSMutableDictionary *_cachedListenerIcons;
	NSMutableDictionary *_cachedListenerSmallIcons;
	CFMutableSetRef _listenerInstances;
    BOOL _hasPerformedFullMetadataCache;
    int _preference_notify_token;
    uint64_t _preferences_change_count;
    volatile OSSpinLock _cacheLock;
}
+ (LAActivator *)sharedInstance;

@property (nonatomic, readonly) LAActivatorVersion version;
@property (nonatomic, readonly, getter=isRunningInsideSpringBoard) BOOL runningInsideSpringBoard;
@property (nonatomic, readonly, getter=isDangerousToSendEvents) BOOL dangerousToSendEvents;

// Listeners

- (id<LAListener>)listenerForEvent:(LAEvent *)event;
- (void)sendEventToListener:(LAEvent *)event;
- (void)sendAbortToListener:(LAEvent *)event;
- (void)sendDeactivateEventToListeners:(LAEvent *)event;

- (id<LAListener>)listenerForName:(NSString *)name;
- (void)registerListener:(id<LAListener>)listener forName:(NSString *)name; // Only available in SpringBoard
- (void)unregisterListenerWithName:(NSString *)name; // Only available in SpringBoard

- (BOOL)hasSeenListenerWithName:(NSString *)name;

// Assignments

- (void)assignEvent:(LAEvent *)event toListenerWithName:(NSString *)listenerName;
- (void)unassignEvent:(LAEvent *)event;
- (NSString *)assignedListenerNameForEvent:(LAEvent *)event;
- (NSArray *)eventsAssignedToListenerWithName:(NSString *)listenerName;

// Events

@property (nonatomic, readonly) NSArray *availableEventNames;
- (BOOL)eventWithNameIsHidden:(NSString *)name;
- (NSArray *)compatibleModesForEventWithName:(NSString *)name;
- (BOOL)eventWithName:(NSString *)eventName isCompatibleWithMode:(NSString *)eventMode;

- (void)registerEventDataSource:(id<LAEventDataSource>)dataSource forEventName:(NSString *)eventName;
- (void)unregisterEventDataSourceWithEventName:(NSString *)eventName;

// Listener Metadata

@property (nonatomic, readonly) NSArray *availableListenerNames;
- (id)infoDictionaryValueOfKey:(NSString *)key forListenerWithName:(NSString *)name;
- (BOOL)listenerWithNameRequiresAssignment:(NSString *)name;
- (NSArray *)compatibleEventModesForListenerWithName:(NSString *)name;
- (BOOL)listenerWithName:(NSString *)listenerName isCompatibleWithMode:(NSString *)eventMode;
- (BOOL)listenerWithName:(NSString *)listenerName isCompatibleWithEventName:(NSString *)eventName;
- (UIImage *)iconForListenerName:(NSString *)listenerName;
- (UIImage *)smallIconForListenerName:(NSString *)listenerName;

// Event Modes

@property (nonatomic, readonly) NSArray *availableEventModes;
@property (nonatomic, readonly) NSString *currentEventMode;

// Blacklisting

@property (nonatomic, readonly) NSString *displayIdentifierForCurrentApplication;
- (BOOL)applicationWithDisplayIdentifierIsBlacklisted:(NSString *)displayIdentifier;
- (void)setApplicationWithDisplayIdentifier:(NSString *)displayIdentifier isBlacklisted:(BOOL)blacklisted;

@end

extern LAActivator *LASharedActivator;

@interface LAActivator (Localization)
- (NSString *)localizedStringForKey:(NSString *)key value:(NSString *)value;

- (NSString *)localizedTitleForEventMode:(NSString *)eventMode;
- (NSString *)localizedTitleForEventName:(NSString *)eventName;
- (NSString *)localizedTitleForListenerName:(NSString *)listenerName;

- (NSString *)localizedGroupForEventName:(NSString *)eventName;
- (NSString *)localizedGroupForListenerName:(NSString *)listenerName;

- (NSString *)localizedDescriptionForEventMode:(NSString *)eventMode;
- (NSString *)localizedDescriptionForEventName:(NSString *)eventName;
- (NSString *)localizedDescriptionForListenerName:(NSString *)listenerName;
@end

// Listeners

@protocol LAListener <NSObject>
@optional

- (void)activator:(LAActivator *)activator didChangeToEventMode:(NSString *)eventMode;

// Incoming events
- (void)activator:(LAActivator *)activator receiveEvent:(LAEvent *)event forListenerName:(NSString *)listenerName;
- (void)activator:(LAActivator *)activator abortEvent:(LAEvent *)event forListenerName:(NSString *)listenerName;
- (void)activator:(LAActivator *)activator receiveDeactivateEvent:(LAEvent *)event;
- (void)activator:(LAActivator *)activator otherListenerDidHandleEvent:(LAEvent *)event;

// Simpler versions
- (void)activator:(LAActivator *)activator receiveEvent:(LAEvent *)event;
- (void)activator:(LAActivator *)activator abortEvent:(LAEvent *)event;

// Metadata (may be cached)
- (NSString *)activator:(LAActivator *)activator requiresLocalizedTitleForListenerName:(NSString *)listenerName;
- (NSString *)activator:(LAActivator *)activator requiresLocalizedDescriptionForListenerName:(NSString *)listenerName;
- (NSString *)activator:(LAActivator *)activator requiresLocalizedGroupForListenerName:(NSString *)listenerName;
- (NSNumber *)activator:(LAActivator *)activator requiresRequiresAssignmentForListenerName:(NSString *)listenerName;
- (NSArray *)activator:(LAActivator *)activator requiresCompatibleEventModesForListenerWithName:(NSString *)listenerName;
- (NSNumber *)activator:(LAActivator *)activator requiresIsCompatibleWithEventName:(NSString *)eventName listenerName:(NSString *)listenerName;
- (id)activator:(LAActivator *)activator requiresInfoDictionaryValueOfKey:(NSString *)key forListenerWithName:(NSString *)listenerName;

// Icons
//  Fast path that supports scale
- (NSData *)activator:(LAActivator *)activator requiresIconDataForListenerName:(NSString *)listenerName scale:(CGFloat *)scale;
- (NSData *)activator:(LAActivator *)activator requiresSmallIconDataForListenerName:(NSString *)listenerName scale:(CGFloat *)scale;
//  Legacy
- (NSData *)activator:(LAActivator *)activator requiresIconDataForListenerName:(NSString *)listenerName;
- (NSData *)activator:(LAActivator *)activator requiresSmallIconDataForListenerName:(NSString *)listenerName;
//  For cases where PNG data isn't available quickly
- (UIImage *)activator:(LAActivator *)activator requiresIconForListenerName:(NSString *)listenerName scale:(CGFloat)scale;
- (UIImage *)activator:(LAActivator *)activator requiresSmallIconForListenerName:(NSString *)listenerName scale:(CGFloat)scale;

@end

// Settings Controllers

#ifndef LA_SETTINGS_CONTROLLER
#define LA_SETTINGS_CONTROLLER(superclass) : superclass
#endif

@interface LASettingsViewController LA_SETTINGS_CONTROLLER(UIViewController)
+ (id)controller;
- (id)init;
@end

@interface LARootSettingsController LA_SETTINGS_CONTROLLER(LASettingsViewController)
@end

@interface LAModeSettingsController LA_SETTINGS_CONTROLLER(LASettingsViewController)
- (id)initWithMode:(NSString *)mode;
@end

@interface LAEventSettingsController LA_SETTINGS_CONTROLLER(LASettingsViewController)
- (id)initWithModes:(NSArray *)modes eventName:(NSString *)eventName;
@end

@interface LAListenerSettingsViewController LA_SETTINGS_CONTROLLER(LASettingsViewController)
@property (nonatomic, copy) NSString *listenerName;
@end

extern NSString * const LAEventModeSpringBoard;
extern NSString * const LAEventModeApplication;
extern NSString * const LAEventModeLockScreen;


extern NSString * const LAEventNameMenuPressSingle;
extern NSString * const LAEventNameMenuPressDouble;
extern NSString * const LAEventNameMenuPressTriple;
extern NSString * const LAEventNameMenuHoldShort;
extern NSString * const LAEventNameMenuHoldLong;

extern NSString * const LAEventNameLockHoldShort;
extern NSString * const LAEventNameLockPressDouble;

extern NSString * const LAEventNameSpringBoardPinch;
extern NSString * const LAEventNameSpringBoardSpread;

extern NSString * const LAEventNameStatusBarSwipeRight;
extern NSString * const LAEventNameStatusBarSwipeLeft;
extern NSString * const LAEventNameStatusBarTapDouble;
extern NSString * const LAEventNameStatusBarTapDoubleLeft;
extern NSString * const LAEventNameStatusBarTapDoubleRight;
extern NSString * const LAEventNameStatusBarTapSingle;
extern NSString * const LAEventNameStatusBarTapSingleLeft;
extern NSString * const LAEventNameStatusBarTapSingleRight;
extern NSString * const LAEventNameStatusBarHold;
extern NSString * const LAEventNameStatusBarHoldLeft;
extern NSString * const LAEventNameStatusBarHoldRight;

extern NSString * const LAEventNameVolumeDownUp;
extern NSString * const LAEventNameVolumeUpDown;
extern NSString * const LAEventNameVolumeDisplayTap;
extern NSString * const LAEventNameVolumeToggleMuteTwice;
extern NSString * const LAEventNameVolumeDownHoldShort;
extern NSString * const LAEventNameVolumeUpHoldShort;
extern NSString * const LAEventNameVolumeDownPress;
extern NSString * const LAEventNameVolumeUpPress;
extern NSString * const LAEventNameVolumeBothPress;

extern NSString * const LAEventNameSlideInFromBottom;
extern NSString * const LAEventNameSlideInFromBottomLeft;
extern NSString * const LAEventNameSlideInFromBottomRight;
extern NSString * const LAEventNameSlideInFromLeft;
extern NSString * const LAEventNameSlideInFromRight;
extern NSString * const LAEventNameStatusBarSwipeDown; // Now a slide gesture in iOS5.0+; extern and name kept for backwards compatibility
#define LAEventNameSlideInFromTop LAEventNameStatusBarSwipeDown
extern NSString * const LAEventNameSlideInFromTopLeft;
extern NSString * const LAEventNameSlideInFromTopRight;

extern NSString * const LAEventNameTwoFingerSlideInFromBottom;
extern NSString * const LAEventNameTwoFingerSlideInFromBottomLeft;
extern NSString * const LAEventNameTwoFingerSlideInFromBottomRight;
extern NSString * const LAEventNameTwoFingerSlideInFromLeft;
extern NSString * const LAEventNameTwoFingerSlideInFromRight;
extern NSString * const LAEventNameTwoFingerSlideInFromTop;
extern NSString * const LAEventNameTwoFingerSlideInFromTopLeft;
extern NSString * const LAEventNameTwoFingerSlideInFromTopRight;

extern NSString * const LAEventNameMotionShake;

extern NSString * const LAEventNameHeadsetButtonPressSingle;
extern NSString * const LAEventNameHeadsetButtonHoldShort;
extern NSString * const LAEventNameHeadsetConnected;
extern NSString * const LAEventNameHeadsetDisconnected;

extern NSString * const LAEventNameLockScreenClockDoubleTap;

extern NSString * const LAEventNamePowerConnected;
extern NSString * const LAEventNamePowerDisconnected;

extern NSString * const LAEventNameThreeFingerTap;
extern NSString * const LAEventNameThreeFingerPinch;
extern NSString * const LAEventNameThreeFingerSpread;

extern NSString * const LAEventNameFourFingerTap;
extern NSString * const LAEventNameFourFingerPinch;
extern NSString * const LAEventNameFourFingerSpread;

extern NSString * const LAEventNameFiveFingerTap;
extern NSString * const LAEventNameFiveFingerPinch;
extern NSString * const LAEventNameFiveFingerSpread;

extern NSString * const LAActivatorAvailableListenersChangedNotification;
extern NSString * const LAActivatorAvailableEventsChangedNotification;
