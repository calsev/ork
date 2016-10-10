/*
This file is part of the ORK_STR library.
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
	return key + BORK(" = \"") + val + BORK("\"");
}


std::ostream& string::export_html(std::ostream&out)const {
	out << text;
	return out;
}


std::ostream& heading::export_html(std::ostream&out)const {
	out << BORK("<h") << level << BORK(" ") << pair::export_html(BORK("class"), style) << BORK(">") << text << BORK("</h") << level << BORK(">\n");
	return out;
}


std::ostream& page_break::export_html(std::ostream&out)const {
	out << BORK("<p style=\"page-break-after: always;\"><!--pagebreak--></p>\n\n");
	return out;
}


std::ostream& image::export_html(std::ostream&out)const {
	out << BORK("<img class=\"") << style << BORK("\" src=\"") << source << BORK("\" ");
	if(!alt_text.empty())out << pair::export_html(BORK("alt"), alt_text) << BORK(" ");
	if(width>0)out << BORK("width=\"") << width << BORK("px\" ");
	if(height>0)out << BORK("height=\"") << height << BORK("px\" ");
	//TODO: Handle border
	//TODO: Handle float
	/*
	style="float:left;width:42px;height:42px;
	*/
	out << pair::export_html(BORK("style"), BORK("border-style:none"));
	out << BORK(">\n");
	return out;
}
/*
<img src = "FADAL%20-%204%20-%20AXIS%20VMC%20MM1.jpg" width = "340px" height = "54px" style = "border-style:none;" / >
*/



bstring padding::export_html()const {
	b_string_stream out;
	out << BORK("padding: ") << top << BORK("px ") << right << BORK("px ") << bottom << BORK("px ") << left << BORK("px;");
	return out.str();
}


style::style(const bstring&name_, const int size_pct_, const int width_, const unsigned flags_) :name(name_), size_pct(size_pct_), width(width_), flags(flags_) {}
style::style(const bstring&name_, const int size_pct_, const int width_, const unsigned flags_, const bstring&color_, const bstring&background_, const padding&pad_)
	: name(name_), size_pct(size_pct_), width(width_), flags(flags_), color(color_), background(background_), pad(pad_) {}
bool style::decorated()const {
	return (flags & (overline | thruline | underline)) != 0;
}
std::ostream& style::export_html(std::ostream&out)const {
	out << name << BORK(" { ");
	out << BORK("font: ");
	out << size_pct << BORK("% ");
	out << (flags&serif ? BORK("serif") : BORK("sans-serif")) << BORK("; ");
	out << BORK("margin: 0; ");
	if(flags&bold)out << BORK("font-weight: bold; ");
	if(width > 0)out << BORK("width: ") << width << BORK("px; ");
	if(flags&italic)out << BORK("font-style: italic; ");
	if(decorated()) {
		out << BORK("text-decoration:");
		bool comma = false;
		if(flags&overline) {
			out << BORK(" overline");
			comma = true;
		}
		if(flags&thruline) {
			out << (comma ? BORK(", ") : BORK("")) << BORK(" thruline");
			comma = true;
		}
		if(flags&underline) {
			out << (comma ? BORK(", ") : BORK("")) << BORK(" underline");
			comma = true;
		}
		out << BORK("; ");
	}
	out << BORK("color: #") << color << BORK("; ");
	if(!background.empty()) {
		out << BORK("background: #") + background + BORK(" !important; ");
	}
	out << pad.export_html();
	out << BORK(" }\n");
	return out;
}


std::ostream& image_style::export_html(std::ostream&out)const {
	out << BORK("img.") << name << BORK(" { margin: 0 auto; ");
	if(alignment == align::left) {
		out << BORK("float: left; ");
	}
	else if(alignment == align::right) {
		out << BORK("float: right; ");
	}
	else {
		out << BORK("clear: both; ");
	}
	out << pad.export_html();
	out << BORK(" }\n");
	return out;
}


div_style::div_style(const bstring&name_, const unsigned width_px_, const align a, const bstring&color)
	:name(name_), width_px(width_px_), alignment(a), back_color(color) {}
std::ostream& div_style::export_html(std::ostream&out)const {
	out << BORK("div.") << name << BORK(" { margin: 0 auto; ");
	if(width_px > 0) {
		out << BORK("width: ") << width_px << BORK("px; ");
	}
	if(alignment == align::left) {
		out << BORK("float: left; ");
	}
	else if(alignment == align::right) {
		out << BORK("float: right; ");
	}
	else {
		out << BORK("clear: both; ");
	}
	if(!back_color.empty()) {
		out << BORK("background-color: #") << back_color << BORK("; ");
	}
	out << BORK("}\n");
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
	out << BORK("table.") << name << BORK(" { ");
	if(border_width > 0 && borders != border::none) {
		if(borders == border::both || borders == border::outside) {
			out << BORK("border: ") << border_width << BORK("px solid #") << color << BORK("; ");
		}
		else {//borders == border::inside;
			//We must mask the outside borders
			out << BORK("border: ") << border_width << BORK("px solid border-style: hidden; ");
		}
	}
	out << BORK("border-collapse: collapse; ");//We have not been able to come up with a scenario where we want '90s-style tables
	if(width_pct > 0) {
		out << BORK("width: ") << width_pct << BORK("%; ");
	}
	if(height_px > 0) {
		out << BORK("height: ") << height_px << BORK("px; ");
	}
	if(padding_px > 0) {
		out << BORK("padding: ") << padding_px << BORK("px; ");
	}
	out << BORK("}\n");

	/*
	Done with the table proper, now for headers, rows, elements
	*/
	if(border_width > 0 && borders != border::none && borders != border::outside) {
	#if 0
	#elif 0
		out << BORK("\ttable.") << name << BORK(" th, td { ");
		out << BORK("border: ") << border_width << BORK("px solid #") << color << BORK("; ");
		out << BORK("}\n");
	#else
		out << BORK("\ttable.") << name << BORK(" tr+tr { ");
		out << BORK("border-top: ") << border_width << BORK("px solid #") << color << BORK("; ");
		out << BORK("}\n");
		out << BORK("\ttable.") << name << BORK(" th+th, td+td { ");
		out << BORK("border-left: ") << border_width << BORK("px solid #") << color << BORK("; ");
		out << BORK("}\n");
	#endif
	}
	return out;
}


line_style::line_style(const bstring&name_, const unsigned width, const align a, const bstring&color_, const unsigned top_px, const unsigned bottom_px, const unsigned height_px_)
	: name(name_), width_pct(width), alignment(a), color(color_), top_pad(top_px), bottom_pad(bottom_px), height_px(height_px_) {}
std::ostream& line_style::export_html(std::ostream&out)const {
	out << BORK("hr.") << name << BORK(" { display: block; clear: both; border: 0 none; margin: 0; ");
	if(width_pct > 0) {
		out << BORK("width: ") << width_pct << BORK("%; ");
	}
	if(top_pad > 0) {
		out << BORK("pad-top: ") << top_pad << BORK("px; ");
		out << BORK("margin-top: ") << top_pad << BORK("px; ");
	}
	if(bottom_pad > 0) {
		out << BORK("pad-bottom: ") << bottom_pad << BORK("px; ");
		out << BORK("margin-bottom: ") << bottom_pad << BORK("px; ");
	}
	out << BORK("height: 0px; ");
	if(height_px > 0) {
		//out << BORK("height: ") << height_px << BORK("px; ");//The hr itself will not print
		out << BORK("border-bottom: solid ") << height_px << BORK("px; ");
	}
	if(alignment == align::left) {
		out << BORK("margin-left: 0; ");
		out << BORK("margin-right: auto; ");
	}
	else if(alignment == align::right) {
		out << BORK("margin-left: auto; ");
		out << BORK("margin-right: 0; ");
	}
	else {
		out << BORK("margin-left: auto; ");
		out << BORK("margin-right: auto; ");
	}
	out << BORK("color: #") << color << BORK("; ");
	out << BORK("border-color: #") << color << BORK("; ");
	out << BORK("background-color: #") << color << BORK("; ");
	out << BORK("noshade: noshade; ");
	out << BORK("}\n");
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
	out << BORK("<style>\n");
	LOOPVI(styles) {
		out << BORK("\t");
		styles[i].export_html(out);
	}
	LOOPVI(image_styles) {
		out << BORK("\t");
		image_styles[i].export_html(out);
	}
	LOOPVI(div_styles) {
		out << BORK("\t");
		div_styles[i].export_html(out);
	}
	LOOPVI(table_styles) {
		out << BORK("\t");
		table_styles[i].export_html(out);
	}
	LOOPVI(line_styles) {
		out << BORK("\t");
		line_styles[i].export_html(out);
	}
	out << BORK("\t@media print{\n");
	LOOPVI(styles) {
		if(!styles[i].background.empty()) {
			out << BORK("\t\t") << styles[i].name << BORK(" { background: #") << styles[i].color << BORK(" !important; ");
			//out << BORK("-webkit-print-color-adjust: exact; ");//Hack for Chrome/Firefox that makes the color black
			out << BORK("}\n");
		}
	}
	LOOPVI(line_styles) {
		out << BORK("\t\thr.") << line_styles[i].name << BORK(" { display: block; }\n");
	}
	out << BORK("\t}\n");
	out << BORK("</style>\n");
	return out;
}


void meta(const bstring&key, const bstring&val, std::ostream&out) {
	out << BORK("<meta ") << pair::export_html(key, val) << BORK(">\n");
}
std::ostream& header::export_html(std::ostream&out)const {
	out << BORK("<head>\n");
	meta(BORK("charset"), encoding, out);
	if(!author.empty())meta(BORK("author"), author, out);
	if(!description.empty())meta(BORK("description"), description, out);
	styles.export_html(out);
	if(!title.empty())out << BORK("<title>") << title << BORK("</title>\n");
	out << BORK("</head>\n");
	return out;
}


std::ostream& line::export_html(std::ostream&out)const {
	out << BORK("<hr class=\"") << style << BORK("\">\n");
	return out;
}


std::ostream& label::export_html(std::ostream&out)const {
#if 0
	out << BORK("<pre>") << text << BORK("</pre>");
#else
	out << text;
#endif
	return out;
}


void table_element::export_html(const exportable*element, const size_t remain_col, std::ostream&out) {
	if(element != nullptr) {
		out << BORK("<td");
		if(element->col_span)out << pair::export_html(BORK(" colspan"), boost::lexical_cast<bstring>(remain_col));
		if(!element->style.empty())out << pair::export_html(BORK(" class"), element->style);
		out << BORK(">");
		element->export_html(out);
	}
	else {
		out << BORK("<td>");
	}
	out << BORK("</td>");
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
	out << BORK("<table");
	if(!_table_style.empty()) {
		out << BORK(" class=\"") << _table_style << BORK("\"");
	}
	else {
		if(padding > 0)out << BORK(" cellpadding=\"") << padding << BORK("\"");
		if(spacing > 0)out << BORK(" cellspacing=\"") << spacing << BORK("\"");
	}
	out << BORK(">\n");
	for(const table::row_type&row : _rows) {
		out << BORK("<tr>");
		LOOPVI(row) {
			const ptr_type&element = row[i];
			table_element::export_html(element.get(), _num_column - i, out);
			if(element && element->col_span)break;
		}
		out << BORK("</tr>\n");
	}
	out << BORK("</table>\n");
	return out;
}


std::ostream& div::export_html(std::ostream&out)const {
	out << BORK("<div class=\"") << style << BORK("\">\n");
	for(const ptr_type&element : elements) {
		if(element)element->export_html(out);
	}
	out << BORK("</div>\n");
	return out;
}


std::ostream& body::export_html(std::ostream&out)const {
	out << BORK("<body>\n");
	for(const div&d : divs) {
		d.export_html(out);
	}
	out << BORK("</body>\n");
	return out;
}


std::ostream& document::export_html(std::ostream&out)const {
	out << BORK("<!DOCTYPE html>\n");//Support HTML 5
	out << BORK("<html>\n");
	head.export_html(out);
	body.export_html(out);
	out << BORK("</html>\n");
	return out;
}


}//namespace html
}//namespace ork
