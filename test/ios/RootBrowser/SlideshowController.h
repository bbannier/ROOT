#import <UIKit/UIKit.h>

@class SlideView;

namespace ROOT_iOS {

class FileContainer;
class Pad;

}

@interface SlideshowController : UIViewController <UIScrollViewDelegate> {
   ROOT_iOS::Pad *pads[2];
   SlideView *padViews[2];//The current and the next in a slide show.

   unsigned visiblePad;
   unsigned nCurrentObject;
   
   ROOT_iOS::FileContainer *fileContainer;
   
   NSTimer *timer;
   IBOutlet UIView *parentView;
   IBOutlet UIView *padParentView;
}

- (id)initWithNibName : (NSString *)nibNameOrNil bundle : (NSBundle *)nibBundleOrNil fileContainer : (ROOT_iOS::FileContainer *)container;

@end
