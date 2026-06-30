#include <wx/wx.h>
#include "main_frame.h"

class MDTrackerApp : public wxApp {
public:
    bool OnInit() override {
        auto* frame = new MainFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MDTrackerApp);
