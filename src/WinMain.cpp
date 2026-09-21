#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <windowsx.h>
#include "BenchmarkData/BenchmarkData.h"
#include "ChartScale.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
	constexpr COLORREF Background = RGB(14, 22, 36), Panel = RGB(23, 35, 53);
	constexpr COLORREF Foreground = RGB(226, 234, 247), Muted = RGB(160, 178, 203), Grid = RGB(53, 70, 93);
	constexpr std::array<COLORREF, 6> Colors = { RGB(83,213,193),RGB(119,166,255),RGB(255,185,105),RGB(214,148,244),RGB(255,130,156),RGB(180,217,104) };
	enum Id { Open = 101, Clear, Export, Help, RunSelect, CaseSelect, SizeSelect, MetricSelect, LogScale, DataTable };
	std::wstring Wide(const std::string& text)
	{
		if (text.empty())return{};
		int n = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
		std::wstring result(static_cast<std::size_t>(n), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), n);
		return result;
	}
	void Fill(HDC dc, RECT r, COLORREF c) { HBRUSH b = CreateSolidBrush(c);FillRect(dc, &r, b);DeleteObject(b); }
	void Line(HDC dc, int x1, int y1, int x2, int y2, COLORREF c, int width = 1)
	{
		HPEN pen = CreatePen(PS_SOLID, width, c); HGDIOBJ old = SelectObject(dc, pen);
		MoveToEx(dc, x1, y1, nullptr);LineTo(dc, x2, y2);SelectObject(dc, old);DeleteObject(pen);
	}
	struct Hover { POINT location; std::wstring label; };
	class Viewer
	{
	public:
		HWND window = nullptr;
		bench::Dataset data;
		void Load(const std::vector<std::filesystem::path>& paths)
		{
			std::wstring errors; std::size_t success = 0;
			SetCursor(LoadCursorW(nullptr, IDC_WAIT));
			for (const auto& path : paths)
			{
				try
				{
					auto rows = bench::ReadCsv(path);
					data.Add(rows); ++success;
				}
				catch (const std::exception& e)
				{
					errors += path.filename().wstring() + L": " + Wide(e.what()) + L"\n";
				}
			}
			Refresh(0); SetCursor(LoadCursorW(nullptr, IDC_ARROW));
			if (!errors.empty()) MessageBoxW(window, (L"Loaded" + std::to_wstring(success) + L" file(s).\n\n" + errors).c_str(), L"CSV import", MB_OK | MB_ICONWARNING);
		}

		LRESULT Message(UINT message, WPARAM w, LPARAM l)
		{
			switch (message)
			{
			case WM_CREATE: CreateControls(); return 0;
			case WM_SIZE: Layout(); return 0;
			case WM_GETMINMAXINFO:
			{
				auto* info = reinterpret_cast<MINMAXINFO*>(l); info->ptMinTrackSize = { Px(1050),Px(720) };return 0;
			}
			case WM_COMMAND:
				if (LOWORD(w) == Open) ChooseFiles();
				else if (LOWORD(w) == Clear) { data.Clear(); Refresh(0); }
				else if (LOWORD(w) == Export) Save();
				else if (LOWORD(w) == Help) ShowHelp();
				else if (LOWORD(w) == LogScale) { hover.clear(); InvalidateRect(window, nullptr, FALSE); }
				else if (HIWORD(w) == CBN_SELCHANGE)
				{
					if (LOWORD(w) == RunSelect) Refresh(1);
					else if (LOWORD(w) == CaseSelect) Refresh(2);
					else if (LOWORD(w) == SizeSelect) Refresh(3);
					else if (LOWORD(w) == MetricSelect) Refresh(4);
				}
				return 0;
			case WM_DROPFILES:
			{
				HDROP drop = reinterpret_cast<HDROP>(w); std::vector<std::filesystem::path> paths;
				const UINT count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
				for (UINT i = 0;i < count;++i)
				{
					UINT len = DragQueryFileW(drop, i, nullptr, 0); std::wstring p(len + 1, L'\0');
					DragQueryFileW(drop, i, p.data(), len + 1); p.resize(len); paths.emplace_back(p);
				}
				DragFinish(drop); Load(paths); return 0;
			}
			case WM_MOUSEMOVE: Mouse(GET_X_LPARAM(l), GET_Y_LPARAM(l));return 0;
			case WM_MOUSELEAVE: hovered.clear(); InvalidateRect(window, &statusRect, FALSE); return 0;
			case WM_ERASEBKGND: return 1;
			case WM_PAINT: Paint(); return 0;
			case WM_CTLCOLORSTATIC:
			{
				HDC dc = reinterpret_cast<HDC>(w); SetBkColor(dc, Background);
				SetTextColor(dc, Foreground);return reinterpret_cast<LRESULT>(backgroundBrush);
			}
			case WM_DESTROY:
				DragAcceptFiles(window, FALSE);
				DeleteObject(font); DeleteObject(titleFont); DeleteObject(backgroundBrush); PostQuitMessage(0); return 0;
			}
			return DefWindowProcW(window, message, w, l);
		}

	private:
		HWND runBox{}, caseBox{}, sizeBox{}, metricBox{}, logBox{}, table{}, contextBox{};
		HWND openButton{}, clearButton{}, exportButton{}, helpButton{};
		HWND runLabel{}, caseLabel{}, sizeLabel{}, metricLabel{};
		HFONT font{}, titleFont{};HBRUSH backgroundBrush{};
		double dpiScale = 1;
		std::vector<bench::Run> runs;
		std::vector<std::string> cases, sizes, metrics;
		bench::Scope scope;
		std::string metric = "ns_per_pair";
		std::vector<bench::Summary> summaries;
		std::vector<bench::Series> series;
		std::vector<Hover> hover;
		std::wstring hovered, status;
		RECT charts{}, statusRect{};

		int Px(int v) const { return static_cast<int>(std::lround(v * dpiScale)); }

		HWND Control(const wchar_t* kind, const wchar_t* text, DWORD style, int id = 0)
		{
			HWND h = CreateWindowExW(0, kind, text, WS_CHILD | WS_VISIBLE | style, 0, 0, 10, 10, window,
				reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), GetModuleHandleW(nullptr), nullptr);
			if (!h)throw std::runtime_error("Cannot create a Windows control.");
			SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE); return h;
		}

		void Text(HDC dc, RECT r, const std::wstring& text, COLORREF color = Foreground, UINT flags = DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER)
		{
			SetTextColor(dc, color); SetBkMode(dc, TRANSPARENT);DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &r, flags | DT_NOPREFIX);
		}

		void CreateControls()
		{
			HDC dc = GetDC(window); dpiScale = GetDeviceCaps(dc, LOGPIXELSX) / 96.0;ReleaseDC(window, dc);
			font = CreateFontW(-Px(14), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
			titleFont = CreateFontW(-Px(23), 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
			backgroundBrush = CreateSolidBrush(Background);
			openButton = Control(L"BUTTON", L"Open CSV...", WS_TABSTOP, Open);
			clearButton = Control(L"BUTTON", L"Clear", WS_TABSTOP, Clear);
			exportButton = Control(L"BUTTON", L"Export table...", WS_TABSTOP, Export);
			helpButton = Control(L"STATIC", L"Help", WS_TABSTOP, Help);
			runLabel = Control(L"STATIC", L"Run / conditions", 0);
			caseLabel = Control(L"STATIC", L"Test case", 0);
			sizeLabel = Control(L"STATIC", L"Requested bytes", 0);
			metricLabel = Control(L"STATIC", L"Sample metric", 0);
			constexpr DWORD combo = WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST;
			runBox = Control(L"COMBOBOX", L"", combo, RunSelect);
			caseBox = Control(L"COMBOBOX", L"", combo, CaseSelect);
			sizeBox = Control(L"COMBOBOX", L"", combo, SizeSelect);
			metricBox = Control(L"COMBOBOX", L"", combo, MetricSelect);
			logBox = Control(L"BUTTON", L"Log10 axis", WS_TABSTOP | BS_AUTOCHECKBOX, LogScale);
			contextBox = Control(L"EDIT", L"", ES_READONLY | ES_AUTOHSCROLL | WS_TABSTOP);
			table = Control(WC_LISTVIEWW, L"", WS_TABSTOP | WS_BORDER | LVS_REPORT | LVS_SINGLESEL, DataTable);
			ListView_SetExtendedListViewStyle(table, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
			ListView_SetBkColor(table, Panel); ListView_SetTextBkColor(table, Panel); ListView_SetTextColor(table, Foreground);
			const wchar_t* headers[] = { L"Backend",L"Min ns/pair", L"Median ns/pair",L"Max ns/pair",L"Time / CRT",L"Samples",L"Source" };
			int widths[] = { 145,140,150,140,130,100,130 };
			for (int i = 0;i < 7;++i)
			{
				LVCOLUMNW column{}; column.mask = LVCF_TEXT | LVCF_WIDTH; column.pszText = const_cast<wchar_t*>(headers[i]); column.cx = Px(widths[i]);
				SendMessageW(table, LVM_INSERTCOLUMNW, static_cast<WPARAM>(i), reinterpret_cast<LPARAM>(&column));
			}
			SendMessageW(runBox, CB_SETDROPPEDWIDTH, Px(950), 0);
			SendMessageW(metricBox, CB_SETDROPPEDWIDTH, Px(300), 0);
			DragAcceptFiles(window, TRUE); Refresh(0); Layout();
		}

		int Index(HWND h) const { return static_cast<int>(SendMessageW(h, CB_GETCURSEL, 0, 0)); }

		std::string Selected(HWND h, const std::vector<std::string>& v) const
		{
			const int i = Index(h);
			return i >= 0 && static_cast<std::size_t>(i) < v.size() ? v[static_cast<std::size_t>(i)] : "";
		}

		void SetOptions(HWND h, const std::vector<std::string>& values, const std::string& previous)
		{
			// Initialize
			SendMessageW(h, CB_RESETCONTENT, 0, 0); int selected = 0;

			// ComboBox String Add 
			for (std::size_t i = 0;i < values.size();++i)
			{
				auto s = Wide(values[i]); SendMessageW(h, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(s.c_str()));
				if (values[i] == previous) selected = static_cast<int>(i);
			}

			// Selected Initialzing
			if (!values.empty())SendMessageW(h, CB_SETCURSEL, static_cast<WPARAM>(selected), 0);

			// If there is no value, deactivate the corresponding window
			EnableWindow(h, !values.empty());
		}

		void Refresh(int level)
		{
			if (level == 0)
			{
				const auto previous = scope.run; runs = data.Runs(); std::vector<std::string> labels;
				int selected = 0;
				for (std::size_t i = 0;i < runs.size();++i)
				{
					labels.push_back(runs[i].label);
					if (runs[i].key == previous)
						selected = static_cast<int>(i);
				}
				SetOptions(runBox, labels, "");
				if (!runs.empty())
					SendMessageW(runBox, CB_SETCURSEL, static_cast<WPARAM>(selected), 0);
			}

			const int ri = Index(runBox);
			scope.run = ri >= 0 && static_cast<std::size_t>(ri) < runs.size() ? runs[static_cast<std::size_t>(ri)].key : "";
			if (level <= 1) { cases = data.Cases(scope.run); SetOptions(caseBox, cases, scope.test); }
			scope.test = Selected(caseBox, cases);
			if (level <= 2) { sizes = data.Sizes(scope);SetOptions(sizeBox, sizes, scope.size); }
			scope.size = Selected(sizeBox, sizes);
			if (level <= 3) { metrics = data.Metrics(scope);SetOptions(metricBox, metrics, metric); }
			metric = Selected(metricBox, metrics);
			summaries = data.Summaries(scope); series = data.Samples(scope, metric);
			status = std::to_wstring(data.SummaryCount()) + L" summary rows  |  " + std::to_wstring(data.SampleCount()) + L" samples  |  " + std::to_wstring(runs.size()) + L"run/condition groups  |  Drop CSV files here";
			SetWindowTextW(contextBox, ri >= 0 ? Wide(runs[static_cast<std::size_t>(ri)].description).c_str() : L"Open summary.csv, samples.csv, or both. Ctrl+O opens files.");
			EnableWindow(exportButton, !summaries.empty()); UpdateTable();
			hovered.clear(); hover.clear(); InvalidateRect(window, nullptr, FALSE);
		}

		void UpdateTable()
		{
			ListView_DeleteAllItems(table); double crt = 0;
			for (const auto& s : summaries) if (s.backend == "CRT") crt = s.median;
			for (std::size_t i = 0;i < summaries.size();++i)
			{
				const auto& s = summaries[i]; std::wstring name = Wide(s.backend);
				LVITEMW item{}; item.mask = LVIF_TEXT; item.iItem = static_cast<int>(i); item.pszText = name.data();
				SendMessageW(table, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&item));
				std::vector<std::string> values = { bench::Format(s.minimum), bench::Format(s.median), bench::Format(s.maximum),
				crt > 0 ? bench::Format(s.median / crt) + "x" : "n/a",std::to_string(s.sampleCount),s.fromSummary ? "summary" : "from samples" };
				for (std::size_t j = 0;j < values.size();++j)
				{
					auto text = Wide(values[j]); LVITEMW sub{}; sub.iSubItem = static_cast<int>(j + 1);sub.pszText = text.data();
					SendMessageW(table, LVM_SETITEMTEXTW, static_cast<WPARAM>(i), reinterpret_cast<LPARAM>(&sub));
				}
			}
		}

		void Layout()
		{
			if (!table) return;
			RECT client{}; GetClientRect(window, &client); int width = client.right, height = client.bottom;
			auto move = [&](HWND h, int x, int y, int w, int hh) {MoveWindow(h, Px(x), Px(y), Px(w), Px(hh), TRUE);};
			move(openButton, 24, 62, 118, 30); move(clearButton, 152, 62, 80, 30);
			move(exportButton, 242, 62, 132, 30); move(helpButton, 384, 62, 75, 30);
			int logicalWidth = static_cast<int>(width / dpiScale), runWidth = std::max(260, logicalWidth - 716);
			move(runLabel, 24, 104, runWidth, 20); move(runBox, 24, 128, runWidth, 320);
			int x = 24 + runWidth + 14;
			move(caseLabel, x, 104, 156, 20); move(caseBox, x, 128, 156, 260); x += 170;
			move(sizeLabel, x, 104, 120, 20); move(sizeBox, x, 128, 120, 260); x += 134;
			move(metricLabel, x, 104, 216, 20); move(metricBox, x, 128, 216, 350); x += 230;
			move(logBox, x, 128, 130, 26);
			MoveWindow(contextBox, Px(24), Px(165), width - Px(48), Px(24), TRUE);
			const int chartBottom = std::max(Px(450), height - Px(247));
			charts = { Px(24), Px(205), width - Px(24),chartBottom };
			MoveWindow(table, Px(24), chartBottom + Px(32), width - Px(48), height - chartBottom - Px(98), TRUE);
			statusRect = { Px(24),height - Px(34), width - Px(24), height - Px(8) };
			hover.clear(); InvalidateRect(window, nullptr, FALSE);
		}

		COLORREF Color(const std::string& backend) const
		{
			auto it = std::find_if(summaries.begin(), summaries.end(), [&](const auto& s) {return s.backend == backend;});
			return Colors[static_cast<std::size_t>(it - summaries.begin()) % Colors.size()];
		}

		void GridLines(HDC dc, RECT plot, const bench::ChartScale& scale)
		{
			for (int i = 0;i <= 5;++i)
			{
				double f = i / 5.0; int y = plot.bottom - static_cast<int>(f * (plot.bottom - plot.top));
				Line(dc, plot.left, y, plot.right, y, Grid);
				RECT r{ plot.left - Px(102), y - Px(10), plot.left - Px(8), y + Px(10) };
				Text(dc, r, Wide(bench::Format(scale.Tick(f))), Muted, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
			}
		}

		void Chart(HDC dc, RECT panel, bool samples)
		{
			Fill(dc, panel, Panel);
			RECT title{ panel.left + Px(16), panel.top + Px(8), panel.right - Px(12), panel.top + Px(33) };
			Text(dc, title, samples ? L"Repeated measurements" : L"Allocator comparison");
			RECT subtitle{ title.left, title.bottom, title.right, title.bottom + Px(22) };
			Text(dc, subtitle, samples ? Wide(metric + " (" + bench::MetricUnit(metric) + ")") : L"Median + min/max range (ns/pair) - lower is faster", Muted);
			if (summaries.empty()) { RECT r = panel; r.top += Px(65); Text(dc, r, L"Open CSV files to view results. ", Muted, DT_CENTER | DT_VCENTER | DT_SINGLELINE); return; }
			const bool log = SendMessageW(logBox, BM_GETCHECK, 0, 0) == BST_CHECKED;
			std::vector<double> values;
			if (samples) { for (const auto& s : series) for (const auto& p : s.points) if (p.value) values.push_back(*p.value); }
			else { for (const auto& s : summaries) { values.push_back(s.minimum); values.push_back(s.median); values.push_back(s.maximum); } }
			if (values.empty()) { RECT r = panel; r.top += Px(65); Text(dc, r, L"No samples for this metric / selection. ", Muted, DT_CENTER | DT_VCENTER | DT_SINGLELINE); return; }
			bench::ChartScale scale(values, log);
			RECT plot{ panel.right + Px(113), panel.top + Px(101), panel.right - Px(24), panel.bottom - Px(39) };
			if (plot.right <= plot.left || plot.bottom <= plot.top) return;
			GridLines(dc, plot, scale);
			auto y = [&](double v) {return plot.bottom - static_cast<int>(*scale.Fraction(v) * (plot.bottom - plot.top));};
			if (log && std::any_of(values.begin(), values.end(), [](double v) {return v <= 0;}))
			{
				RECT note{ title.left, subtitle.bottom, title.right, subtitle.bottom + Px(20) };
				Text(dc, note, L"Zero values hidden on log axis. Use linear to inspect.", RGB(255, 185, 105));
			}
			else if (samples)
			{
				std::wstring legend;
				for (const auto& s : series)
				{
					if (!legend.empty())
						legend += L"  |   ";
					legend += Wide(s.backend);
				}
				RECT r{ title.left, subtitle.bottom, title.right, subtitle.bottom + Px(22) };
				Text(dc, r, legend, Muted);
			}
			if (!samples)
			{
				double step = static_cast<double>(plot.right - plot.left) / summaries.size();
				for (std::size_t i = 0;i < summaries.size();++i)
				{
					const auto& s = summaries[i]; int x = plot.left + static_cast<int>(step * (i + .5));
					int half = std::max(1, std::min(Px(34), static_cast<int>(step * .3)));
					if (scale.Fraction(s.median))
					{
						RECT bar{ x - half, y(s.median), x + half, plot.bottom }; Fill(dc, bar, Color(s.backend));
						Line(dc, x - half, y(s.median), x + half, y(s.median), Color(s.backend), 2);
						hover.push_back({ {x,y(s.median)},Wide(s.backend + ": median " + bench::Format(s.median) + " ns/pair | min " + bench::Format(s.minimum) + " | max " + bench::Format(s.maximum)) });
					}
					if (scale.Fraction(s.maximum))
					{
						const int top = y(s.maximum), bottom = scale.Fraction(s.minimum) ? y(s.minimum) : plot.bottom;
						Line(dc, x, top, x, bottom, Foreground);
						Line(dc, x - Px(6), top, x + Px(6), top, Foreground);
						if (scale.Fraction(s.minimum)) Line(dc, x - Px(6), bottom, x + Px(6), bottom, Foreground);
					}
					RECT label{ x - static_cast<int>(step / 2),plot.bottom + Px(7), x + static_cast<int>(step / 2),plot.bottom + Px(30) };
					Text(dc, label, Wide(s.backend), Color(s.backend), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
				}
			}
			else
			{
				double first = 0, last = 0;bool found = false;
				for (const auto& s : series) for (const auto& p : s.points)
				{
					if (!found) { first = last = p.round; found = true; }
					first = std::min(first, p.round); last = std::max(last, p.round);
				}
				auto x = [&](double round) {return first == last ? (plot.left + plot.right) / 2 : plot.left + static_cast<int>((round - first) / (last - first) * (plot.right - plot.left));};
				for (const auto& s : series)
				{
					bool previous = false; POINT prev{};
					for (const auto& p : s.points)
					{
						if (!p.value || !scale.Fraction(*p.value)) { previous = false; continue; }
						POINT pt{ x(p.round),y(*p.value) };
						if (previous) Line(dc, prev.x, prev.y, pt.x, pt.y, Color(s.backend), 2);
						RECT dot{ pt.x - Px(3), pt.y - Px(3),pt.x + Px(3),pt.y + Px(3) }; Fill(dc, dot, Color(s.backend));
						hover.push_back({ pt,Wide(s.backend + " | round " + bench::Format(p.round) + " | " + metric + " = " + bench::Format(*p.value)) });
						previous = true; prev = pt;
					}
				}
				RECT label{ plot.left,plot.bottom + Px(7),plot.right,plot.bottom + Px(30) };
				Text(dc, label, Wide("Round" + bench::Format(first) + " ... " + bench::Format(last)), Muted, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		}

		void Paint()
		{
			PAINTSTRUCT ps{}; HDC dc = BeginPaint(window, &ps); RECT r{}; GetClientRect(window, &r);
			if (r.right > 0 && r.bottom > 0)
			{
				HDC memory = CreateCompatibleDC(dc); HBITMAP bitmap = CreateCompatibleBitmap(dc, r.right, r.bottom);
				HGDIOBJ oldBitmap = SelectObject(memory, bitmap); HGDIOBJ oldFont = SelectObject(memory, font);
				Fill(memory, r, Background); SelectObject(memory, titleFont);
				RECT title{ Px(24),Px(14),r.right - Px(24),Px(49) }; Text(memory, title, L"Memory Benchmark Viewer");
				SelectObject(memory, font);
				hover.clear(); const int mid = (charts.left + charts.right) / 2;
				Chart(memory, { charts.left,charts.top,mid - Px(8),charts.bottom }, false);
				Chart(memory, { mid + Px(8),charts.top,charts.right,charts.bottom }, true);
				RECT note{ charts.left,charts.bottom + Px(4),charts.right,charts.bottom + Px(27) };
				Text(memory, note, L"Time / CRT = median / CRT median. Above 1x is slower. Min/Max is the observed range, not a confidence interval.", Muted);
				Text(memory, statusRect, hovered.empty() ? status : hovered, Muted);
				BitBlt(dc, 0, 0, r.right, r.bottom, memory, 0, 0, SRCCOPY);
				SelectObject(memory, oldFont); SelectObject(memory, oldBitmap); DeleteObject(bitmap); DeleteDC(memory);
			}
			EndPaint(window, &ps);
		}

		void Mouse(int x, int y)
		{
			TRACKMOUSEEVENT tracking{ sizeof(LPTRACKMOUSEEVENT),TME_LEAVE,window,0 };
			TrackMouseEvent(&tracking);
			std::wstring next; int best = Px(14) * Px(14);
			for (const auto& h : hover)
			{
				const int dx = x - h.location.x, dy = y - h.location.y;
				const int distance = dx * dx + dy * dy;
				if (distance < best) { best = distance;next = h.label; }
			}
			if (next != hovered) { hovered = next; InvalidateRect(window, &statusRect, FALSE); }
		}

		void ChooseFiles()
		{
			std::vector<wchar_t> buffer(65536);
			OPENFILENAMEW dialog{}; dialog.lStructSize = sizeof(dialog); dialog.hwndOwner = window;
			dialog.lpstrFilter = L"Benchmark CSV (*.csv)\0*.csv\0All files (*.*)\0*.*\0\0";
			dialog.lpstrFile = buffer.data(); dialog.nMaxFile = static_cast<DWORD>(buffer.size());
			dialog.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
			if (!GetOpenFileNameW(&dialog))
			{
				if (CommDlgExtendedError())
					MessageBoxW(window, L"File dialog failed. Try Selecting fewer files.", L"Open", MB_OK | MB_ICONERROR);
				return;
			}
			std::vector<std::filesystem::path> paths;
			std::filesystem::path first(buffer.data());
			const wchar_t* next = buffer.data() + first.native().size() + 1;
			if (*next == L'\0') paths.push_back(first);
			else
			{
				while (*next)
				{
					std::wstring name(next);
					paths.push_back(first / name);
					next += name.size() + 1;
				}
			}
			Load(paths);
		}

		void Save()
		{
			std::array<wchar_t, 32768> buffer{}; const std::wstring defaultName = L"Benchmaek - comparison.csv";
			std::copy(defaultName.begin(), defaultName.end(), buffer.begin());
			OPENFILENAMEW dialog{}; dialog.lStructSize = sizeof(dialog); dialog.hwndOwner = window;
			dialog.lpstrFile = buffer.data(); dialog.nMaxFile = static_cast<DWORD>(buffer.size()); dialog.lpstrDefExt = L"csv";
			dialog.lpstrFilter = L"CSV (*.csv)\0*.csv\0\0"; dialog.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
			if (!GetSaveFileNameW(&dialog))
			{
				if (CommDlgExtendedError())
					MessageBoxW(window, L"Save dialog failed", L"Export", MB_OK | MB_ICONERROR);
				return;
			}
			try
			{
				const auto text = data.Export(scope);
				std::ofstream out(std::filesystem::path(buffer.data()), std::ios::binary);
				if (!out)throw std::runtime_error("Cannot create export file.");
				out.write(text.data(), static_cast<std::streamsize>(text.size()));out.close();
				if (!out) throw std::runtime_error("Export write failed");
				MessageBoxW(window, L"Comparison table saved.", L"Export", MB_OK | MB_ICONINFORMATION);
			}
			catch (const std::exception& e)
			{
				MessageBoxW(window, Wide(e.what()).c_str(), L"Export error", MB_OK | MB_ICONERROR);
			}
		}

		void ShowHelp()
		{
			MessageBoxW(window,
				L"1. Open summary.csv samples.csv or both (Ctrl + 0). \n"
				, L"Benchmark Viewer - Help", MB_OK | MB_ICONINFORMATION);
		}
	};

	LRESULT CALLBACK WindowProc(HWND h, UINT m, WPARAM w, LPARAM l)
	{
		Viewer* app = reinterpret_cast<Viewer*>(GetWindowLongPtrW(h, GWLP_USERDATA));
		if (m == WM_NCCREATE)
		{
			app = static_cast<Viewer*>(reinterpret_cast<CREATESTRUCTW*>(l)->lpCreateParams); app->window = h;
			SetWindowLongPtrW(h, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
		}
		try
		{
			return app ? app->Message(m, w, l) : DefWindowProcW(h, m, w, l);
		}
		catch (const std::exception& e)
		{
			MessageBoxW(h, Wide(e.what()).c_str(), L"Benchmark Viewer error", MB_OK | MB_ICONERROR);
			if (m == WM_CREATE)
				return -1;
			return 0;
		}
	}
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show)
{
	SetProcessDPIAware();
	INITCOMMONCONTROLSEX controls{ sizeof(INITCOMMONCONTROLSEX), ICC_LISTVIEW_CLASSES };
	if (!InitCommonControlsEx(&controls))return 1;
	WNDCLASSEXW wc{};wc.cbSize = sizeof(wc); wc.lpfnWndProc = WindowProc; wc.hInstance = instance;
	wc.lpszClassName = L"StandaloneBenchmarkViewer"; wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	if (!RegisterClassExW(&wc)) return 1;
	Viewer app;
	HWND h = CreateWindowExW(WS_EX_CONTROLPARENT, wc.lpszClassName, L"Memory Benchmark Viewer", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, 1320, 900, nullptr, nullptr, instance, &app);

	if (!h)
	{
		const DWORD error_code = h ? ERROR_SUCCESS : GetLastError();
		wchar_t error_string[64];
		swprintf_s(error_string, _countof(error_string), L"CreateWindowExW is Failed : %lu", error_code);
		MessageBoxW(nullptr, error_string, L"Fatal : Initialize is Failed", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(h, show);
	UpdateWindow(h);

	int argc = 0; LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argv)
	{
		std::vector<std::filesystem::path> paths;
		for (int i = 1;i < argc;++i)
			paths.emplace_back(argv[i]);
		LocalFree(argv);
		if (!paths.empty())
			app.Load(paths);
	}
	MSG message{};

	int result;

	while ((result = static_cast<int>(GetMessageW(&message, nullptr, 0, 0))) > 0)
	{
		if (message.message == WM_KEYDOWN && message.wParam == '0' && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			SendMessageW(h, WM_COMMAND, Open, 0);
			continue;
		}

		if (!IsDialogMessageW(h, &message))
		{
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}

	return result == -1 ? 1 : static_cast<int>(message.wParam);
}