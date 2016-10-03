/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include<string>
#include<sstream>
#include"boost/lexical_cast.hpp"
#include"ork/html.hpp"

namespace ork {
namespace html {


std::ostream& pair::export_html(std::ostream&out)const {
	out << export_html(key, value);
	return out;
}

bstring pair::export_html(const bstring&key, const bstring&val) {
	return key + BTXT(" = \"") + val + BTXT("\"");
}


std::ostream& string::export_html(std::ostream&out)const {
	out << text;
	return out;
}


std::ostream& heading::export_html(std::ostream&out)const {
	out << BTXT("<h") << level << BTXT(" ") << pair::export_html(BTXT("class"), style) << BTXT(">") << text << BTXT("</h") << level << BTXT(">\n");
	return out;
}


std::ostream& page_break::export_html(std::ostream&out)const {
	out << BTXT("<p style=\"page-break-after: always;\"><!--pagebreak--></p>\n\n");
	return out;
}


std::ostream& image::export_html(std::ostream&out)const {
	out << BTXT("<img class=\"") << style << BTXT("\" src=\"") << source << BTXT("\" ");
	if(!alt_text.empty())out << pair::export_html(BTXT("alt"), alt_text) << BTXT(" ");
	if(width>0)out << BTXT("width=\"") << width << BTXT("px\" ");
	if(height>0)out << BTXT("height=\"") << height << BTXT("px\" ");
	//TODO: Handle border
	//TODO: Handle float
	/*
	style="float:left;width:42px;height:42px;
	*/
	out << pair::export_html(BTXT("style"), BTXT("border-style:none"));
	out << BTXT(">\n");
	return out;
}
/*
<img src = "FADAL%20-%204%20-%20AXIS%20VMC%20MM1.jpg" width = "340px" height = "54px" style = "border-style:none;" / >
*/



bstring padding::export_html()const {
	b_string_stream out;
	out << BTXT("padding: ") << top << BTXT("px ") << right << BTXT("px ") << bottom << BTXT("px ") << left << BTXT("px;");
	return out.str();
}


style::style(const bstring&name_, const int size_pct_, const int width_, const unsigned flags_) :name(name_), size_pct(size_pct_), width(width_), flags(flags_) {}
style::style(const bstring&name_, const int size_pct_, const int width_, const unsigned flags_, const bstring&color_, const bstring&background_, const padding&pad_)
	: name(name_), size_pct(size_pct_), width(width_), flags(flags_), color(color_), background(background_), pad(pad_) {}
bool style::decorated()const {
	return (flags & (overline | thruline | underline)) != 0;
}
std::ostream& style::export_html(std::ostream&out)const {
	out << name << BTXT(" { ");
	out << BTXT("font: ");
	out << size_pct << BTXT("% ");
	out << (flags&serif ? BTXT("serif") : BTXT("sans-serif")) << BTXT("; ");
	out << BTXT("margin: 0; ");
	if(flags&bold)out << BTXT("font-weight: bold; ");
	if(width > 0)out << BTXT("width: ") << width << BTXT("px; ");
	if(flags&italic)out << BTXT("font-style: italic; ");
	if(decorated()) {
		out << BTXT("text-decoration:");
		bool comma = false;
		if(flags&overline) {
			out << BTXT(" overline");
			comma = true;
		}
		if(flags&thruline) {
			out << (comma ? BTXT(", ") : BTXT("")) << BTXT(" thruline");
			comma = true;
		}
		if(flags&underline) {
			out << (comma ? BTXT(", ") : BTXT("")) << BTXT(" underline");
			comma = true;
		}
		out << BTXT("; ");
	}
	out << BTXT("color: #") << color << BTXT("; ");
	if(!background.empty()) {
		out << BTXT("background: #") + background + BTXT(" !important; ");
	}
	out << pad.export_html();
	out << BTXT(" }\n");
	return out;
}


std::ostream& image_style::export_html(std::ostream&out)const {
	out << BTXT("img.") << name << BTXT(" { margin: 0 auto; ");
	if(alignment == align::left) {
		out << BTXT("float: left; ");
	}
	else if(alignment == align::right) {
		out << BTXT("float: right; ");
	}
	else {
		out << BTXT("clear: both; ");
	}
	out << pad.export_html();
	out << BTXT(" }\n");
	return out;
}


div_style::div_style(const bstring&name_, const unsigned width_px_, const align a, const bstring&color)
	:name(name_), width_px(width_px_), alignment(a), back_color(color) {}
std::ostream& div_style::export_html(std::ostream&out)const {
	out << BTXT("div.") << name << BTXT(" { margin: 0 auto; ");
	if(width_px > 0) {
		out << BTXT("width: ") << width_px << BTXT("px; ");
	}
	if(alignment == align::left) {
		out << BTXT("float: left; ");
	}
	else if(alignment == align::right) {
		out << BTXT("float: right; ");
	}
	else {
		out << BTXT("clear: both; ");
	}
	if(!back_color.empty()) {
		out << BTXT("background-color: #") << back_color << BTXT("; ");
	}
	out << BTXT("}\n");
	return out;
}


table_style::table_style(
	const bstring&name_
	, const unsigned border_width_
	, const border bord
	, const bstring&color_
	, const unsigned width_pct_
	, const unsigned height_px_
	, const unsigned padding_px_
)
	: name(name_)
	, border_width(border_width_)
	, borders(bord)
	, color(color_)
	, width_pct(width_pct_)
	, height_px(height_px_)
	, padding_px(padding_px_)
{}
std::ostream& table_style::export_html(std::ostream&out)const {
	out << BTXT("table.") << name << BTXT(" { ");
	if(border_width > 0 && borders != border::none) {
		if(borders == border::both || borders == border::outside) {
			out << BTXT("border: ") << border_width << BTXT("px solid #") << color << BTXT("; ");
		}
		else {//borders == border::inside;
			//We must mask the outside borders
			out << BTXT("border: ") << border_width << BTXT("px solid border-style: hidden; ");
		}
	}
	out << BTXT("border-collapse: collapse; ");//We have not been able to come up with a scenario where we want '90s-style tables
	if(width_pct > 0) {
		out << BTXT("width: ") << width_pct << BTXT("%; ");
	}
	if(height_px > 0) {
		out << BTXT("height: ") << height_px << BTXT("px; ");
	}
	if(padding_px > 0) {
		out << BTXT("padding: ") << padding_px << BTXT("px; ");
	}
	out << BTXT("}\n");

	/*
	Done with the table proper, now for headers, rows, elements
	*/
	if(border_width > 0 && borders != border::none && borders != border::outside) {
	#if 0
	#elif 0
		out << BTXT("\ttable.") << name << BTXT(" th, td { ");
		out << BTXT("border: ") << border_width << BTXT("px solid #") << color << BTXT("; ");
		out << BTXT("}\n");
	#else
		out << BTXT("\ttable.") << name << BTXT(" tr+tr { ");
		out << BTXT("border-top: ") << border_width << BTXT("px solid #") << color << BTXT("; ");
		out << BTXT("}\n");
		out << BTXT("\ttable.") << name << BTXT(" th+th, td+td { ");
		out << BTXT("border-left: ") << border_width << BTXT("px solid #") << color << BTXT("; ");
		out << BTXT("}\n");
	#endif
	}
	return out;
}


line_style::line_style(const bstring&name_, const unsigned width, const align a, const bstring&color_, const unsigned top_px, const unsigned bottom_px, const unsigned height_px_)
	: name(name_), width_pct(width), alignment(a), color(color_), top_pad(top_px), bottom_pad(bottom_px), height_px(height_px_) {}
std::ostream& line_style::export_html(std::ostream&out)const {
	out << BTXT("hr.") << name << BTXT(" { display: block; clear: both; border: 0 none; margin: 0; ");
	if(width_pct > 0) {
		out << BTXT("width: ") << width_pct << BTXT("%; ");
	}
	if(top_pad > 0) {
		out << BTXT("pad-top: ") << top_pad << BTXT("px; ");
		out << BTXT("margin-top: ") << top_pad << BTXT("px; ");
	}
	if(bottom_pad > 0) {
		out << BTXT("pad-bottom: ") << bottom_pad << BTXT("px; ");
		out << BTXT("margin-bottom: ") << bottom_pad << BTXT("px; ");
	}
	out << BTXT("height: 0px; ");
	if(height_px > 0) {
		//out << BTXT("height: ") << height_px << BTXT("px; ");//The hr itself will not print
		out << BTXT("border-bottom: solid ") << height_px << BTXT("px; ");
	}
	if(alignment == align::left) {
		out << BTXT("margin-left: 0; ");
		out << BTXT("margin-right: auto; ");
	}
	else if(alignment == align::right) {
		out << BTXT("margin-left: auto; ");
		out << BTXT("margin-right: 0; ");
	}
	else {
		out << BTXT("margin-left: auto; ");
		out << BTXT("margin-right: auto; ");
	}
	out << BTXT("color: #") << color << BTXT("; ");
	out << BTXT("border-color: #") << color << BTXT("; ");
	out << BTXT("background-color: #") << color << BTXT("; ");
	out << BTXT("noshade: noshade; ");
	out << BTXT("}\n");
	return out;
}
/*
hr {
border-width: 1px;
border-top: 1px solid;
padding: 0;
margin: 1em 0;
margin-top: 0.5em;
margin-bottom: 0.5em;
margin-left: auto;
margin-right: auto;
border-style: inset;
}
*/


std::ostream& style_set::export_html(std::ostream&out)const {
	out << BTXT("<style>\n");
	LOOPVI(styles) {
		out << BTXT("\t");
		styles[i].export_html(out);
	}
	LOOPVI(image_styles) {
		out << BTXT("\t");
		image_styles[i].export_html(out);
	}
	LOOPVI(div_styles) {
		out << BTXT("\t");
		div_styles[i].export_html(out);
	}
	LOOPVI(table_styles) {
		out << BTXT("\t");
		table_styles[i].export_html(out);
	}
	LOOPVI(line_styles) {
		out << BTXT("\t");
		line_styles[i].export_html(out);
	}
	out << BTXT("\t@media print{\n");
	LOOPVI(styles) {
		if(!styles[i].background.empty()) {
			out << BTXT("\t\t") << styles[i].name << BTXT(" { background: #") << styles[i].color << BTXT(" !important; ");
			//out << BTXT("-webkit-print-color-adjust: exact; ");//Hack for Chrome/Firefox that makes the color black
			out << BTXT("}\n");
		}
	}
	LOOPVI(line_styles) {
		out << BTXT("\t\thr.") << line_styles[i].name << BTXT(" { display: block; }\n");
	}
	out << BTXT("\t}\n");
	out << BTXT("</style>\n");
	return out;
}


void meta(const bstring&key, const bstring&val, std::ostream&out) {
	out << BTXT("<meta ") << pair::export_html(key, val) << BTXT(">\n");
}
std::ostream& header::export_html(std::ostream&out)const {
	out << BTXT("<head>\n");
	meta(BTXT("charset"), encoding, out);
	if(!author.empty())meta(BTXT("author"), author, out);
	if(!description.empty())meta(BTXT("description"), description, out);
	styles.export_html(out);
	if(!title.empty())out << BTXT("<title>") << title << BTXT("</title>\n");
	out << BTXT("</head>\n");
	return out;
}


std::ostream& line::export_html(std::ostream&out)const {
	out << BTXT("<hr class=\"") << style << BTXT("\">\n");
	return out;
}


std::ostream& label::export_html(std::ostream&out)const {
#if 0
	out << BTXT("<pre>") << text << BTXT("</pre>");
#else
	out << text;
#endif
	return out;
}


void table_element::export_html(const exportable*element, const size_t remain_col, std::ostream&out) {
	if(element != nullptr) {
		out << BTXT("<td");
		if(element->col_span)out << pair::export_html(BTXT(" colspan"), boost::lexical_cast<bstring>(remain_col));
		if(!element->style.empty())out << pair::export_html(BTXT(" class"), element->style);
		out << BTXT(">");
		element->export_html(out);
	}
	else {
		out << BTXT("<td>");
	}
	out << BTXT("</td>");
}


void table::insert_row() {
	_rows.push_back(row_type());
	_rows.back().resize(_num_column);
}
void table::insert_label(const bstring&style, const bstring&text) {
	insert_row();
	_rows.back()[0].reset(new label(style, text));
}
table::row_type&table::at(const size_t index) {
	return _rows.at(index);
}
const table::row_type&table::at(const size_t index)const {
	return _rows.at(index);
}
table::row_type&table::back() {
	return _rows.back();
}
const table::row_type&table::back()const {
	return _rows.back();
}
std::ostream& table::export_html(std::ostream&out)const {
	out << BTXT("<table");
	if(!_table_style.empty()) {
		out << BTXT(" class=\"") << _table_style << BTXT("\"");
	}
	else {
		if(padding > 0)out << BTXT(" cellpadding=\"") << padding << BTXT("\"");
		if(spacing > 0)out << BTXT(" cellspacing=\"") << spacing << BTXT("\"");
	}
	out << BTXT(">\n");
	for(const table::row_type&row : _rows) {
		out << BTXT("<tr>");
		LOOPVI(row) {
			const ptr_type&element = row[i];
			table_element::export_html(element.get(), _num_column - i, out);
			if(element && element->col_span)break;
		}
		out << BTXT("</tr>\n");
	}
	out << BTXT("</table>\n");
	return out;
}


std::ostream& div::export_html(std::ostream&out)const {
	out << BTXT("<div class=\"") << style << BTXT("\">\n");
	for(const ptr_type&element : elements) {
		if(element)element->export_html(out);
	}
	out << BTXT("</div>\n");
	return out;
}


std::ostream& body::export_html(std::ostream&out)const {
	out << BTXT("<body>\n");
	for(const div&d : divs) {
		d.export_html(out);
	}
	out << BTXT("</body>\n");
	return out;
}


std::ostream& document::export_html(std::ostream&out)const {
	out << BTXT("<!DOCTYPE html>\n");//Support HTML 5
	out << BTXT("<html>\n");
	head.export_html(out);
	body.export_html(out);
	out << BTXT("</html>\n");
	return out;
}


}//namespace html
}//namespace ork
