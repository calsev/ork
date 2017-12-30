/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include<vector>
#include"ork/ork.hpp"

#if ORK_MSC
#pragma warning(push)
#pragma warning(disable : 4251) //DLL interface for X needs DLL interface for Y
#endif

namespace ork {


enum class align {
	  left
	, right
	, center
};


enum class border {
	  none
	, inside
	, outside
	, both
};


namespace html {


//Base class for html stuff
struct ORK_ORK_API exportable {
public:
	bstring style = BORK("");
	bool col_span = false;//Used only if this is inside a table
public:
	virtual ~exportable() {}
	exportable() {}
	explicit exportable(const bstring&style_) :style(style_) {}
	exportable(const bstring&style_, const bool span) :style(style_), col_span(span) {}
public:
	virtual std::ostream& export_html(std::ostream&out)const = 0;
};


struct ORK_ORK_API pair : public exportable {
public:
	bstring key;
	bstring value;
public:
	pair(const bstring&key_, const bstring&val) :key(key_), value(val) {}
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
	static bstring export_html(const bstring&key, const bstring&val);
};


struct ORK_ORK_API string : public exportable {
public:
	bstring text = BORK("");
public:
	string() {}
	string(const bstring&style, const bstring&text_) :exportable(style), text(text_) {}
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API heading : public string {
public:
	unsigned level = 1;
public:
	heading(const bstring&style, const bstring&text, const unsigned level_) :string(style, text), level(level_) {}
	heading(const bstring&style, const bstring&text, const unsigned level_, const unsigned pct_width) :string(style, text), level(level_) {}
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API page_break : public exportable {
public:
public:
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API padding {
public:
	int top = 0;
	int right = 0;
	int bottom = 0;
	int left = 0;
public:
	padding() {}
	padding(const int t, const int r, const int b, const int l) :top(t), right(r), bottom(b), left(l) {}
public:
	bstring export_html()const;
};


struct ORK_ORK_API image : public exportable {
public:
	bstring source = BORK("");
	bstring alt_text = BORK("");
	unsigned width = 0;
	unsigned height = 0;
public:
	image() {}
	image(const bstring&src, const bstring&alt, const bstring&image_style, const unsigned w, const unsigned h) :exportable(image_style), source(src), alt_text(alt), width(w), height(h) {}
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API style : public exportable {
public:
	static const unsigned bold = 0x1<<0;
	static const unsigned serif = 0x1<<1;
	static const unsigned overline = 0x1 << 2;
	static const unsigned thruline = 0x1 << 3;
	static const unsigned underline = 0x1 << 4;
	static const unsigned italic = 0x1 << 5;
public:
	bstring name = BORK("");
	int size_pct = 100;
	int width = 0;
	unsigned flags = 0;
	bstring color = BORK("000000");
	bstring background = BORK("");
	padding pad;
public:
	style(const bstring&name_, const int size_pct_, const int width_, const unsigned flags_);
	style(const bstring&name_, const int size_pct_, const int width_, const unsigned flags_, const bstring&color_, const bstring&background_, const padding&pad_);
public:
	bool decorated()const;
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API image_style : public exportable {
public:
	bstring name = BORK("");
	align alignment = align::left;
	padding pad;
public:
	image_style(const bstring&name_, const align a, const padding&pad_) : name(name_), alignment(a), pad(pad_) {}
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API div_style : public exportable {
public:
	bstring name = BORK("");
	unsigned width_px = 0;
	align alignment = align::left;
	bstring back_color = BORK("");
public:
	div_style(const bstring&name_, const unsigned width_px_, const align a, const bstring&color);
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API table_style : public exportable {
public:
	bstring name = BORK("");
	unsigned border_width = 0;
	border borders;
	bool only_bottom = false;
	bstring color = BORK("000000");
	unsigned width_pct = 0;
	unsigned height_px = 0;
	unsigned padding_px = 0;
public:
	table_style(
		  const bstring&name_
		, const unsigned border_width_
		, const border bord
		, const bstring&color_
		, const unsigned width_pct_
		, const unsigned height_px_
		, const unsigned padding_px_
	);
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API line_style : public exportable {
public:
	bstring name = BORK("");
	unsigned width_pct = 0;
	align alignment = align::left;
	bstring color = BORK("000000");
	unsigned top_pad = 0;
	unsigned bottom_pad = 0;
	unsigned height_px = 0;
public:
	line_style(const bstring&name_, const unsigned width, const align a, const bstring&color_, const unsigned top_px, const unsigned bottom_px, const unsigned height_px_);
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API style_set : public exportable {
public:
	std::vector<html::style>styles;
	std::vector<image_style>image_styles;
	std::vector<div_style>div_styles;
	std::vector<table_style>table_styles;
	std::vector<line_style>line_styles;
public:
	style_set() {}
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API header : public exportable {
public:
	bstring author = BORK("");
	bstring description = BORK("");
	bstring encoding = BORK("UTF-8");
	bstring title = BORK("");
	style_set styles;
public:
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API line : public exportable {
public:
	//unsigned width_px = 80;
	//bool align_left = false;
public:
	line(const bstring&line_style_/*, const unsigned width, const bool left*/) :exportable(line_style_)/*, width_px(width), align_left(left)*/ {}
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API label : public exportable {
public:
	bstring text;
public:
	label(const bstring&style, const bstring&text_) :exportable(style, true), text(text_) {}
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API table_element {
	static void export_html(const exportable*element, const size_t remain_col, std::ostream&out);
};


struct ORK_ORK_API table : public exportable {
public:
	typedef std::unique_ptr<exportable>ptr_type;
	typedef std::vector<ptr_type>row_type;
public:
	unsigned padding = 0;
	unsigned spacing = 0;
protected:
	bstring _table_style = BORK("");
	size_t _num_column;
	std::vector<row_type>_rows;
public:
	table(const bstring&style, const unsigned num_column) :_table_style(style), _num_column(num_column) {}
	table(const bstring&style, const unsigned num_column, const unsigned pad, const unsigned space) :_table_style(style), _num_column(num_column), padding(pad), spacing(space) {}
	ORK_MOVE_ONLY_5(table, padding, spacing, _table_style, _num_column, _rows);
public:
	size_t num_column()const {
		return _num_column;
	}
	void insert_row();
	void insert_label(const bstring&style, const bstring&text);
	row_type&at(const size_t index);
	const row_type&at(const size_t index)const;
	row_type&back();
	const row_type&back()const;
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API div : public exportable {
public:
	typedef std::unique_ptr<exportable>ptr_type;
public:
	std::vector<ptr_type>elements;
public:
	div(const bstring&div_style) :exportable(div_style) {}
	ORK_MOVE_ONLY_1(div, elements);
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API body : public exportable {
public:
	std::vector<div>divs;
public:
	body() {}
	ORK_MOVE_ONLY_1(body, divs);
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


struct ORK_ORK_API document : public exportable {
public:
	header head;
	html::body body;
public:
	document() {}
	document(const header&head_, html::body&&body_) :head(head_), body(std::move(body_)) {}
	ORK_MOVE_ONLY_2(document, head, body);
public:
	virtual std::ostream& export_html(std::ostream&out)const override;
};


}//namespace html
}//namespace ork

#if ORK_MSC
#pragma warning(pop)
#endif
