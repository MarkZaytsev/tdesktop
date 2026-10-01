/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "core/style_overrides.h"

#include "base/options.h"
#include "styles/style_chat.h"
#include "styles/style_chat_helpers.h"
#include "styles/style_dialogs.h"
#include "styles/style_iv.h"
#include "styles/style_media_view.h"

namespace Core {
namespace {

constexpr auto kMinSize = 8;
constexpr auto kMaxSize = 40;
constexpr auto kMinWidthPercent = 50;
constexpr auto kMaxWidthPercent = 400;

base::options::option<int> OptionMessageFontSize({
	.id = "message-font-size",
	.name = "Message font size",
	.description = "Chat message text size in px at 100% scale, "
		"0 for the default.",
	.restartRequired = true,
});

base::options::option<int> OptionChatListFontSize({
	.id = "chat-list-font-size",
	.name = "Chat list font size",
	.description = "Chat list name, preview and date size in px "
		"at 100% scale, 0 for the default.",
	.restartRequired = true,
});

base::options::option<int> OptionMessageWidthPercent({
	.id = "message-width-percent",
	.name = "Message width",
	.description = "Message bubble max width in percent of the default, "
		"0 for the default.",
	.restartRequired = true,
});

template <typename Value>
void Replace(const Value &target, Value with) {
	// Generated style objects are mutable, st:: only exposes const refs.
	const_cast<Value&>(target) = std::move(with);
}

void Override(
		const base::options::option<int> &option,
		int scale,
		std::initializer_list<const style::font*> targets) {
	const auto size = option.value();
	if (size < kMinSize || size > kMaxSize) {
		return;
	}
	for (const auto target : targets) {
		const auto &original = *target;
		Replace(original, style::font(
			style::ConvertScale(size, scale),
			original->flags(),
			original->family()));
	}
}

void OverrideMessageWidth() {
	const auto percent = OptionMessageWidthPercent.value();
	if (percent < kMinWidthPercent || percent > kMaxWidthPercent) {
		return;
	}
	const auto width = st::msgMaxWidth * percent / 100;
	Replace(st::msgMaxWidth, width);
	for (const auto markdown : {
		&st::messageMarkdown,
		&st::messageMarkdownOut,
		&st::messageMarkdownSelected,
		&st::messageMarkdownOutSelected,
	}) {
		Replace(markdown->pageMaxWidth, width);
	}
}

} // namespace

void ApplyStyleOverrides(int scale) {
	// Copied from historyTextStyle by value while the styles start.
	Override(OptionMessageFontSize, scale, {
		&st::msgFont,
		&st::historyTextStyle.font,
		&st::messageTextStyle.font,
		&st::historyComposeField.style.font,
		&st::defaultComposeFilesField.style.font,
		&st::mediaviewCaptionStyle.font,
	});
	Override(OptionChatListFontSize, scale, {
		&st::dialogsNameFont,
		&st::dialogsNameStyle.font,
		&st::dialogsTextFont,
		&st::dialogsTextStyle.font,
		&st::dialogsDateFont,
	});
	OverrideMessageWidth();
}

} // namespace Core
