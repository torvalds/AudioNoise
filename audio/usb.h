// Dummy USB effect
enum usb_output {
	LR_Wet, LR_Dry, LR_WetDry
};

struct {
	enum usb_output output;
} usb;

static struct effect usb_effect = {
	.name = "USB",
	.short_name = "USB",
};
