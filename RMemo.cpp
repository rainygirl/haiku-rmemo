/*
 * R Memo -- notes that sit on the desktop and look like notes.
 *
 * Each note is a borderless window that draws itself: a coloured sheet with a
 * header strip and a folded bottom-right corner. The header is the drag
 * handle and the fold is the resize grip, so nothing here relies on the
 * window decorator -- which is the point, a note with a title bar reads as a
 * dialog, not as a note.
 *
 * Closing the last note quits the application, and a launch with nothing
 * saved opens one empty note, so there is never a running instance with no
 * visible window and never a visible window with no way back.
 *
 * Distributed under the terms of the MIT License.
 */

#include <Alert.h>
#include <Application.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <List.h>
#include <LocaleRoster.h>
#include <MenuItem.h>
#include <Message.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <Screen.h>
#include <String.h>
#include <TextView.h>
#include <View.h>
#include <Window.h>

#include <stdio.h>
#include <string.h>


static const char* const kAppSignature = "application/x-vnd.RMemo";
static const char* const kSettingsDirName = "RMemo";
static const char* const kSettingsFileName = "notes";

static const uint32 kMsgNewNote		= 'nwnt';
static const uint32 kMsgSetColor	= 'stcl';

static const float kHeaderHeight	= 24.0f;
static const float kFoldSize		= 16.0f;
static const float kPadding			= 8.0f;
static const float kButtonSize		= 14.0f;

static const float kMinWidth		= 140.0f;
static const float kMinHeight		= 100.0f;
static const float kDefaultWidth	= 240.0f;
static const float kDefaultHeight	= 200.0f;

// Each new note lands down-right of the previous one so a fresh one is never
// hidden exactly behind the note it was created from.
static const float kCascadeStep		= 26.0f;


// #pragma mark - strings


enum string_id {
	kStrNewNote = 0, kStrClose, kStrColor,
	kStrDiscardTitle, kStrDiscardText, kStrKeep, kStrDiscard,
	kStrYellow, kStrPink, kStrGreen, kStrBlue, kStrOrange, kStrPurple,
	kStrWhite,
	kStringCount
};

static const char* const kStringsEn[kStringCount] = {
	"New note", "Close", "Colour",
	"R Memo", "Throw this note away?", "Keep", "Throw away",
	"Yellow", "Pink", "Green", "Blue", "Orange", "Purple", "White"
};
static const char* const kStringsKo[kStringCount] = {
	"새 메모", "닫기", "색상",
	"R Memo", "이 메모를 버릴까요?", "그대로 두기", "버리기",
	"노랑", "분홍", "초록", "파랑", "주황", "보라", "흰색"
};
static const char* const kStringsJa[kStringCount] = {
	"新しいメモ", "閉じる", "色",
	"R Memo", "このメモを捨てますか？", "残す", "捨てる",
	"黄色", "ピンク", "緑", "青", "オレンジ", "紫", "白"
};
static const char* const kStringsZhHans[kStringCount] = {
	"新建便签", "关闭", "颜色",
	"R Memo", "要丢弃这张便签吗？", "保留", "丢弃",
	"黄色", "粉色", "绿色", "蓝色", "橙色", "紫色", "白色"
};
static const char* const kStringsZhHant[kStringCount] = {
	"新增便條", "關閉", "顏色",
	"R Memo", "要丟棄這張便條嗎？", "保留", "丟棄",
	"黃色", "粉紅色", "綠色", "藍色", "橙色", "紫色", "白色"
};
static const char* const kStringsRu[kStringCount] = {
	"Новая заметка", "Закрыть", "Цвет",
	"R Memo", "Выбросить эту заметку?", "Оставить", "Выбросить",
	"Жёлтый", "Розовый", "Зелёный", "Синий", "Оранжевый", "Фиолетовый",
	"Белый"
};
static const char* const kStringsTr[kStringCount] = {
	"Yeni not", "Kapat", "Renk",
	"R Memo", "Bu not atılsın mı?", "Kalsın", "At",
	"Sarı", "Pembe", "Yeşil", "Mavi", "Turuncu", "Mor", "Beyaz"
};
static const char* const kStringsId[kStringCount] = {
	"Catatan baru", "Tutup", "Warna",
	"R Memo", "Buang catatan ini?", "Simpan", "Buang",
	"Kuning", "Merah muda", "Hijau", "Biru", "Oranye", "Ungu", "Putih"
};
static const char* const kStringsPl[kStringCount] = {
	"Nowa notatka", "Zamknij", "Kolor",
	"R Memo", "Wyrzucić tę notatkę?", "Zachowaj", "Wyrzuć",
	"Żółty", "Różowy", "Zielony", "Niebieski", "Pomarańczowy", "Fioletowy",
	"Biały"
};
static const char* const kStringsHu[kStringCount] = {
	"Új jegyzet", "Bezárás", "Szín",
	"R Memo", "Eldobja ezt a jegyzetet?", "Megtartás", "Eldobás",
	"Sárga", "Rózsaszín", "Zöld", "Kék", "Narancssárga", "Lila", "Fehér"
};
static const char* const kStringsDe[kStringCount] = {
	"Neue Notiz", "Schließen", "Farbe",
	"R Memo", "Diese Notiz wegwerfen?", "Behalten", "Wegwerfen",
	"Gelb", "Rosa", "Grün", "Blau", "Orange", "Lila", "Weiß"
};
static const char* const kStringsFr[kStringCount] = {
	"Nouvelle note", "Fermer", "Couleur",
	"R Memo", "Jeter cette note ?", "Conserver", "Jeter",
	"Jaune", "Rose", "Vert", "Bleu", "Orange", "Violet", "Blanc"
};
static const char* const kStringsVi[kStringCount] = {
	"Ghi chú mới", "Đóng", "Màu sắc",
	"R Memo", "Bỏ ghi chú này?", "Giữ lại", "Bỏ",
	"Vàng", "Hồng", "Xanh lá", "Xanh dương", "Cam", "Tím", "Trắng"
};

static const char* const kStringsIt[kStringCount] = {
	"Nuova nota", "Chiudi", "Colore",
	"R Memo", "Buttare via questa nota?", "Conserva", "Butta via",
	"Giallo", "Rosa", "Verde", "Blu", "Arancione", "Viola", "Bianco"
};

static const char* const kStringsEs[kStringCount] = {
	"Nota nueva", "Cerrar", "Color",
	"R Memo", "¿Tirar esta nota?", "Conservar", "Tirar",
	"Amarillo", "Rosa", "Verde", "Azul", "Naranja", "Morado", "Blanco"
};

// Matched against the locale's preferred languages, longest code first so the
// two written forms of Chinese are told apart before the bare "zh" catch-all
// can claim either.
struct language_entry {
	const char*			code;
	const char* const*	strings;
};

static const language_entry kLanguages[] = {
	{ "zh_Hant", kStringsZhHant },
	{ "zh_Hans", kStringsZhHans },
	{ "zh_TW",   kStringsZhHant },
	{ "zh_HK",   kStringsZhHant },
	{ "zh_MO",   kStringsZhHant },
	{ "zh",      kStringsZhHans },
	{ "ko",      kStringsKo },
	{ "ja",      kStringsJa },
	{ "ru",      kStringsRu },
	{ "tr",      kStringsTr },
	{ "id",      kStringsId },
	{ "pl",      kStringsPl },
	{ "hu",      kStringsHu },
	{ "de",      kStringsDe },
	{ "fr",      kStringsFr },
	{ "it",      kStringsIt },
	{ "es",      kStringsEs },
	{ "vi",      kStringsVi },
	{ "en",      kStringsEn }
};

static const int32 kLanguageCount
	= sizeof(kLanguages) / sizeof(kLanguages[0]);

static const char* const* sStrings = kStringsEn;

// Set once the application itself is going away. Closing a note throws it
// away and is worth confirming; quitting the application saves the notes and
// is not, so without this flag a shutdown asked about every note on screen.
static bool sQuitting = false;

static inline const char*
T(string_id id)
{
	return sStrings[id];
}


// #pragma mark - colours


struct note_color {
	rgb_color	sheet;
	string_id	name;
};

static const note_color kNoteColors[] = {
	{ { 254, 243, 166, 255 }, kStrYellow },
	{ { 255, 202, 212, 255 }, kStrPink },
	{ { 201, 240, 205, 255 }, kStrGreen },
	{ { 199, 226, 255, 255 }, kStrBlue },
	{ { 255, 216, 168, 255 }, kStrOrange },
	{ { 226, 210, 255, 255 }, kStrPurple },
	{ { 246, 246, 244, 255 }, kStrWhite }
};

static const int32 kNoteColorCount
	= sizeof(kNoteColors) / sizeof(kNoteColors[0]);


// Shades a colour towards black (factor < 1) or white (factor > 1). The
// header strip, the border and the fold are all derived from the sheet colour
// this way, so adding a colour to the table above needs nothing else.
static rgb_color
shade(rgb_color base, float factor)
{
	rgb_color result = base;
	float channel[3] = { (float)base.red, (float)base.green, (float)base.blue };

	for (int32 i = 0; i < 3; i++) {
		float value = factor <= 1.0f
			? channel[i] * factor
			: 255.0f - (255.0f - channel[i]) / factor;
		if (value < 0)
			value = 0;
		if (value > 255)
			value = 255;
		channel[i] = value;
	}

	result.red = (uint8)channel[0];
	result.green = (uint8)channel[1];
	result.blue = (uint8)channel[2];
	return result;
}


// #pragma mark - NoteView


class NoteView : public BView {
public:
							NoteView(BRect frame, int32 colorIndex);

	virtual	void			AttachedToWindow();
	virtual	void			Draw(BRect updateRect);
	virtual	void			FrameResized(float width, float height);
	virtual	void			MouseDown(BPoint where);
	virtual	void			MouseUp(BPoint where);
	virtual	void			MouseMoved(BPoint where, uint32 code,
								const BMessage* dragMessage);

			void			SetColorIndex(int32 index);
			int32			ColorIndex() const { return fColorIndex; }
			BTextView*		Text() const { return fText; }

private:
			enum drag_mode {
				kNone = 0,
				kMoving,
				kResizing
			};

			BRect			_CloseRect() const;
			BRect			_NewRect() const;
			BRect			_ColorRect() const;
			BRect			_FoldRect() const;
			void			_Layout();
			void			_ApplyColors();
			void			_ShowColorMenu(BPoint where);

			BTextView*		fText;
			int32			fColorIndex;
			drag_mode		fDragMode;
			BPoint			fDragOffset;
			// The three header buttons are drawn, not real controls, so the
			// tooltip is swapped by hand as the pointer crosses them. Tracked
			// so it is only set when it actually changes -- reassigning it on
			// every mouse move makes it flicker.
			int32			fHoverButton;
};


NoteView::NoteView(BRect frame, int32 colorIndex)
	:
	BView(frame, "note", B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS),
	fText(NULL),
	fColorIndex(colorIndex),
	fDragMode(kNone),
	fHoverButton(-1)
{
	if (fColorIndex < 0 || fColorIndex >= kNoteColorCount)
		fColorIndex = 0;

	BRect textFrame(0, 0, 10, 10);
	fText = new BTextView(textFrame, "text", textFrame, B_FOLLOW_NONE,
		B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS);
	fText->SetStylable(false);
	fText->SetWordWrap(true);
	AddChild(fText);
}


void
NoteView::AttachedToWindow()
{
	_ApplyColors();
	_Layout();
	fText->MakeFocus(true);
}


void
NoteView::_ApplyColors()
{
	rgb_color sheet = kNoteColors[fColorIndex].sheet;
	rgb_color ink = { 44, 42, 38, 255 };

	SetViewColor(sheet);
	SetLowColor(sheet);

	fText->SetViewColor(sheet);
	fText->SetLowColor(sheet);
	// SetFontAndColor with a NULL font keeps the face and changes only the
	// colour, and it has to be applied to the existing text as well as to
	// what gets typed next.
	fText->SetFontAndColor(be_plain_font, B_FONT_ALL, &ink);

	Invalidate();
	fText->Invalidate();
}


void
NoteView::SetColorIndex(int32 index)
{
	if (index < 0 || index >= kNoteColorCount || index == fColorIndex)
		return;

	fColorIndex = index;
	_ApplyColors();
}


BRect
NoteView::_ColorRect() const
{
	float top = (kHeaderHeight - kButtonSize) / 2;
	return BRect(kPadding, top, kPadding + kButtonSize, top + kButtonSize);
}


BRect
NoteView::_CloseRect() const
{
	float top = (kHeaderHeight - kButtonSize) / 2;
	float right = Bounds().right - kPadding;
	return BRect(right - kButtonSize, top, right, top + kButtonSize);
}


BRect
NoteView::_NewRect() const
{
	BRect close = _CloseRect();
	close.OffsetBy(-(kButtonSize + 8), 0);
	return close;
}


BRect
NoteView::_FoldRect() const
{
	BRect bounds = Bounds();
	return BRect(bounds.right - kFoldSize, bounds.bottom - kFoldSize,
		bounds.right, bounds.bottom);
}


void
NoteView::_Layout()
{
	BRect bounds = Bounds();
	BRect frame(kPadding, kHeaderHeight + 2,
		bounds.right - kPadding, bounds.bottom - kFoldSize + 4);

	if (frame.Width() < 10 || frame.Height() < 10)
		return;

	fText->MoveTo(frame.LeftTop());
	fText->ResizeTo(frame.Width(), frame.Height());
	// The text rect is inset inside the view rect; leaving them equal puts the
	// caret hard against the edge of the sheet.
	BRect textRect(0, 0, frame.Width() - 4, frame.Height());
	fText->SetTextRect(textRect);
}


void
NoteView::FrameResized(float width, float height)
{
	BView::FrameResized(width, height);
	_Layout();
	Invalidate();
}


void
NoteView::Draw(BRect updateRect)
{
	BRect bounds = Bounds();
	rgb_color sheet = kNoteColors[fColorIndex].sheet;
	rgb_color header = shade(sheet, 0.90f);
	rgb_color edge = shade(sheet, 0.72f);
	rgb_color glyph = shade(sheet, 0.42f);

	SetHighColor(sheet);
	FillRect(bounds);

	// Header strip: the drag handle, and the only thing that stands in for a
	// title bar.
	SetHighColor(header);
	FillRect(BRect(bounds.left, bounds.top, bounds.right, kHeaderHeight - 1));
	SetHighColor(edge);
	StrokeLine(BPoint(bounds.left, kHeaderHeight - 1),
		BPoint(bounds.right, kHeaderHeight - 1));

	// Colour swatch.
	BRect colorRect = _ColorRect();
	SetHighColor(sheet);
	FillEllipse(colorRect);
	SetHighColor(edge);
	StrokeEllipse(colorRect);

	SetPenSize(1.6f);
	SetHighColor(glyph);

	// Plus.
	BRect newRect = _NewRect();
	StrokeLine(BPoint(newRect.left + 2, newRect.top + newRect.Height() / 2),
		BPoint(newRect.right - 2, newRect.top + newRect.Height() / 2));
	StrokeLine(BPoint(newRect.left + newRect.Width() / 2, newRect.top + 2),
		BPoint(newRect.left + newRect.Width() / 2, newRect.bottom - 2));

	// Cross.
	BRect closeRect = _CloseRect();
	closeRect.InsetBy(2.5f, 2.5f);
	StrokeLine(closeRect.LeftTop(), closeRect.RightBottom());
	StrokeLine(closeRect.LeftBottom(), closeRect.RightTop());

	SetPenSize(1.0f);

	// Folded corner. It doubles as the resize grip, which is why it is drawn
	// as a lifted flap rather than the usual diagonal hatch: a fold is
	// something you expect to be able to pull.
	BRect fold = _FoldRect();
	BPoint flap[3];
	flap[0] = BPoint(fold.left, fold.bottom);
	flap[1] = BPoint(fold.right, fold.top);
	flap[2] = BPoint(fold.right, fold.bottom);
	SetHighColor(shade(sheet, 0.82f));
	FillPolygon(flap, 3);
	SetHighColor(edge);
	StrokeLine(flap[0], flap[1]);

	// Outline last so it sits over everything.
	SetHighColor(edge);
	StrokeRect(bounds);
}


void
NoteView::MouseDown(BPoint where)
{
	if (_CloseRect().Contains(where)) {
		Window()->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	if (_NewRect().Contains(where)) {
		be_app->PostMessage(kMsgNewNote);
		return;
	}

	if (_ColorRect().Contains(where)) {
		_ShowColorMenu(where);
		return;
	}

	if (_FoldRect().Contains(where)) {
		fDragMode = kResizing;
		SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
		return;
	}

	if (where.y < kHeaderHeight) {
		fDragMode = kMoving;
		// Remember where inside the window the pointer grabbed it. Moving the
		// window by a frame-to-frame delta drifts, because each move changes
		// what the next local point means; anchoring to a fixed offset does
		// not.
		fDragOffset = ConvertToScreen(where) - Window()->Frame().LeftTop();
		SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
		return;
	}
}


void
NoteView::MouseUp(BPoint where)
{
	fDragMode = kNone;
}


void
NoteView::MouseMoved(BPoint where, uint32 code, const BMessage* dragMessage)
{
	if (fDragMode == kNone) {
		int32 hover = -1;
		if (_ColorRect().Contains(where))
			hover = kStrColor;
		else if (_NewRect().Contains(where))
			hover = kStrNewNote;
		else if (_CloseRect().Contains(where))
			hover = kStrClose;

		if (hover != fHoverButton) {
			fHoverButton = hover;
			if (hover < 0)
				SetToolTip((const char*)NULL);
			else
				SetToolTip(T((string_id)hover));
		}
		return;
	}

	BPoint screen = ConvertToScreen(where);

	if (fDragMode == kMoving) {
		Window()->MoveTo(screen - fDragOffset);
		return;
	}

	BRect frame = Window()->Frame();
	float width = screen.x - frame.left;
	float height = screen.y - frame.top;

	if (width < kMinWidth)
		width = kMinWidth;
	if (height < kMinHeight)
		height = kMinHeight;

	Window()->ResizeTo(width, height);
}


void
NoteView::_ShowColorMenu(BPoint where)
{
	BPopUpMenu* menu = new BPopUpMenu("colors", false, false);

	for (int32 i = 0; i < kNoteColorCount; i++) {
		BMessage* message = new BMessage(kMsgSetColor);
		message->AddInt32("index", i);
		BMenuItem* item = new BMenuItem(T(kNoteColors[i].name), message);
		item->SetMarked(i == fColorIndex);
		menu->AddItem(item);
	}

	BPoint screen = ConvertToScreen(where);
	BMenuItem* chosen = menu->Go(screen, false, true);
	if (chosen != NULL && chosen->Message() != NULL) {
		int32 index = 0;
		if (chosen->Message()->FindInt32("index", &index) == B_OK)
			SetColorIndex(index);
	}

	delete menu;
}


// #pragma mark - NoteWindow


class NoteWindow : public BWindow {
public:
							NoteWindow(BRect frame, int32 colorIndex,
								const char* text);

	virtual	bool			QuitRequested();

			void			Store(BMessage* into) const;

private:
			NoteView*		fNote;
};


NoteWindow::NoteWindow(BRect frame, int32 colorIndex, const char* text)
	:
	// No border: the whole point is that a note should not look like a
	// dialog. Everything a decorator would provide -- dragging, resizing,
	// closing -- is drawn and handled by NoteView instead.
	BWindow(frame, "R Memo", B_NO_BORDER_WINDOW_LOOK,
		B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE),
	fNote(NULL)
{
	fNote = new NoteView(Bounds(), colorIndex);
	AddChild(fNote);

	if (text != NULL && text[0] != '\0')
		fNote->Text()->SetText(text);
}


bool
NoteWindow::QuitRequested()
{
	// Closing a note throws it away, so an accidental click on a note with
	// something written on it should not be silently destructive.
	const char* text = fNote->Text()->Text();
	if (!sQuitting && text != NULL && text[0] != '\0') {
		BAlert* alert = new BAlert(T(kStrDiscardTitle), T(kStrDiscardText),
			T(kStrKeep), T(kStrDiscard), NULL, B_WIDTH_AS_USUAL,
			B_WARNING_ALERT);
		alert->SetShortcut(0, B_ESCAPE);
		if (alert->Go() == 0)
			return false;
	}

	if (!sQuitting) {
		// Tell the application this note is gone; it quits when the last one
		// is. During shutdown the list is needed intact for saving, so it is
		// left alone.
		BMessage notice('nclo');
		notice.AddPointer("note", this);
		be_app->PostMessage(&notice);
	}

	return true;
}


void
NoteWindow::Store(BMessage* into) const
{
	BMessage note;
	note.AddRect("frame", Frame());
	note.AddInt32("color", fNote->ColorIndex());
	note.AddString("text", fNote->Text()->Text());
	into->AddMessage("note", &note);
}


// #pragma mark - NoteApp


class NoteApp : public BApplication {
public:
							NoteApp();

	virtual	void			ReadyToRun();
	virtual	void			MessageReceived(BMessage* message);
	virtual	bool			QuitRequested();

private:
			void			_NewNote();
			void			_AddNote(BRect frame, int32 colorIndex,
								const char* text);
			void			_NoteClosed(NoteWindow* note);
			status_t		_SettingsPath(BPath& path, bool create) const;
			void			_Load();
			void			_Save();

			BList			fNotes;
			BPoint			fNextPosition;
};


NoteApp::NoteApp()
	:
	BApplication(kAppSignature),
	fNextPosition(120, 120)
{
}


void
NoteApp::ReadyToRun()
{
	_Load();

	// A running application with no window would have no way to reach it, so
	// an empty set always becomes one empty note.
	if (fNotes.CountItems() == 0)
		_NewNote();
}


void
NoteApp::_NewNote()
{
	BScreen screen;
	BRect screenFrame = screen.Frame();

	BRect frame(fNextPosition.x, fNextPosition.y,
		fNextPosition.x + kDefaultWidth, fNextPosition.y + kDefaultHeight);

	// Walk back to the top-left once the cascade would put a note off screen,
	// rather than opening it somewhere the user cannot reach.
	if (frame.right > screenFrame.right || frame.bottom > screenFrame.bottom) {
		fNextPosition.Set(120, 120);
		frame.OffsetTo(fNextPosition);
	}

	fNextPosition += BPoint(kCascadeStep, kCascadeStep);

	int32 colorIndex = 0;
	if (fNotes.CountItems() > 0) {
		NoteWindow* last = (NoteWindow*)fNotes.ItemAt(fNotes.CountItems() - 1);
		BMessage stored;
		last->Store(&stored);
		BMessage note;
		if (stored.FindMessage("note", &note) == B_OK)
			note.FindInt32("color", &colorIndex);
	}

	_AddNote(frame, colorIndex, NULL);
}


void
NoteApp::_AddNote(BRect frame, int32 colorIndex, const char* text)
{
	NoteWindow* window = new NoteWindow(frame, colorIndex, text);
	fNotes.AddItem(window);
	window->Show();
}


void
NoteApp::_NoteClosed(NoteWindow* note)
{
	fNotes.RemoveItem(note);

	if (fNotes.CountItems() == 0)
		PostMessage(B_QUIT_REQUESTED);
}


void
NoteApp::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgNewNote:
			_NewNote();
			break;

		case 'nclo':
		{
			void* pointer = NULL;
			if (message->FindPointer("note", &pointer) == B_OK)
				_NoteClosed((NoteWindow*)pointer);
			break;
		}

		default:
			BApplication::MessageReceived(message);
			break;
	}
}


bool
NoteApp::QuitRequested()
{
	// Runs before the windows are torn down, which is the only point where
	// their contents can still be read.
	_Save();
	sQuitting = true;
	return BApplication::QuitRequested();
}


status_t
NoteApp::_SettingsPath(BPath& path, bool create) const
{
	status_t status = find_directory(B_USER_SETTINGS_DIRECTORY, &path, create);
	if (status != B_OK)
		return status;

	status = path.Append(kSettingsDirName);
	if (status != B_OK)
		return status;

	if (create) {
		status = create_directory(path.Path(), 0755);
		if (status != B_OK)
			return status;
	}

	return path.Append(kSettingsFileName);
}


void
NoteApp::_Load()
{
	BPath path;
	if (_SettingsPath(path, false) != B_OK)
		return;

	BFile file(path.Path(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;

	BMessage saved;
	if (saved.Unflatten(&file) != B_OK)
		return;

	BMessage note;
	for (int32 i = 0; saved.FindMessage("note", i, &note) == B_OK; i++) {
		BRect frame(120, 120, 120 + kDefaultWidth, 120 + kDefaultHeight);
		int32 colorIndex = 0;
		const char* text = NULL;

		note.FindRect("frame", &frame);
		note.FindInt32("color", &colorIndex);
		note.FindString("text", &text);

		_AddNote(frame, colorIndex, text);
		fNextPosition = frame.LeftTop() + BPoint(kCascadeStep, kCascadeStep);
	}
}


void
NoteApp::_Save()
{
	BPath path;
	if (_SettingsPath(path, true) != B_OK)
		return;

	BMessage saved;
	for (int32 i = 0; i < fNotes.CountItems(); i++) {
		NoteWindow* window = (NoteWindow*)fNotes.ItemAt(i);
		if (window->Lock()) {
			window->Store(&saved);
			window->Unlock();
		}
	}

	BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK)
		return;

	saved.Flatten(&file);
}


// #pragma mark -


// Picks the string table from the locale's preferred languages. The whole
// list is walked, not just the first entry, so a user whose first choice is a
// language this app does not carry still gets their second one instead of
// falling straight back to English.
static void
_ChooseLanguage()
{
	BMessage preferred;
	if (BLocaleRoster::Default()->GetPreferredLanguages(&preferred) != B_OK)
		return;

	const char* language = NULL;
	for (int32 i = 0;
			preferred.FindString("language", i, &language) == B_OK; i++) {
		if (language == NULL)
			continue;

		// Haiku writes these with an underscore, but a hyphen turns up in
		// environment overrides, so both spellings are accepted.
		char code[32];
		strncpy(code, language, sizeof(code) - 1);
		code[sizeof(code) - 1] = '\0';
		for (char* c = code; *c != '\0'; c++) {
			if (*c == '-')
				*c = '_';
		}

		for (int32 j = 0; j < kLanguageCount; j++) {
			size_t length = strlen(kLanguages[j].code);
			if (strncmp(code, kLanguages[j].code, length) != 0)
				continue;
			// Only match on a whole subtag: "ko" must not claim "kok".
			if (code[length] != '\0' && code[length] != '_')
				continue;

			sStrings = kLanguages[j].strings;
			return;
		}
	}
}


int
main(void)
{
	_ChooseLanguage();

	NoteApp app;
	app.Run();
	return 0;
}
