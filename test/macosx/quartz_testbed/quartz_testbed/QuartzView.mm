#import "QuartzView.h"

#import "QuartzTest.h"

@implementation QuartzView {
   QuartzTest *test;
}

- (id)initWithFrame:(NSRect)frame
{
   if (self = [super initWithFrame : frame]) {
      // Initialization code here.
      test = new QuartzTest(frame.size.width, frame.size.height);
   }
    
   return self;
}

- (void) awakeFromNib
{
   test = new QuartzTest(300, 300);
}

- (void) drawRect : (NSRect)dirtyRect
{
   // Drawing code here.
   NSGraphicsContext *nsContext = [NSGraphicsContext currentContext];
   CGContextRef cgContext = (CGContextRef)[nsContext graphicsPort];
   
   CGContextTranslateCTM(cgContext, 0.f, dirtyRect.size.height);
   CGContextScaleCTM(cgContext, 1.f, -1.f);
   
   test->SetContext(cgContext);
   test->SetPadSizes(dirtyRect.size.width, dirtyRect.size.height);
   test->Draw();
}

- (void) dealloc
{
   delete test;
}

@end
