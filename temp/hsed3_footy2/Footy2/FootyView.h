/**
 * @file FootyView.h
 * @brief Footyのビュー管理を行うCFootyViewクラスの宣言
 * @author Shinji Watanabe
 * @version 1.0
 */

#pragma once

#include <windows.h>
#include "FontObjects.h"
#include "FootyStatus.h"
#include "DoubleBuffering.h"
#include "FootyDoc.h"
#include "Caret.h"
#ifndef WM_MOUSEWHEEL
#	include "zmouse.h"						/*マウスホイールメッセージ*/
#endif	/*WM_MOUSEWHEEL*/

#define LINEWIDTH_DEFAULT	50
#define RULERHEIGHT_DEFAULT	10

class CFootyView{
//publicメンバ関数//////////////////////////////////////////////////////////
public:
	CFootyView();
	~CFootyView();

	/*ビューの生成*/
	bool Create(HWND hWnd,HINSTANCE hInstance,
				CFontObjects *pFonts,CFootyView *pView,
				CFootyDoc *pDocument,CFootyStatus *pStatus,
				int nFootyID,int nViewID);

	/*ビットマップの読み込み*/
	static void LoadLineIcons(HINSTANCE hInstance);
	static void ReleaseLineIcons();
	
	/*取得*/
	inline HWND GetWnd(){return m_hWnd;}
	inline bool IsFocused(){return m_bIsFocused;}
	inline int GetRulerHeight(){return m_nRulerHeight;}
	inline int GetLineCountWidth(){return m_nLineCountWidth;}
	inline size_t GetVisibleLines(){return m_nVisibleLines;}
	inline size_t GetVisibleColumns(){return m_nVisibleColumns;}
	inline void SetShiftLock(bool bLocked){m_bShiftLocked = bLocked;}
	inline bool IsShiftLocked(){return m_bShiftLocked;}
	inline int GetMarkVisible(){return m_nMarkVisible;}

	/*GUI関係の処理*/
	void SetVisible(bool bVisible);
	inline bool SetVisibleCols(){
		return MoveWin(m_x,m_y,m_nWidth,m_nHeight,false);
	}
	void SetLineWidth(int nWidth);
	void SetRulerHeight(int nHeight);
	bool MoveWin(int x,int y,int nWidth,int nHeight,bool bRefresh = true);
	bool Refresh(bool bSendOther = true);
	void SetFocus(){::SetFocus(m_hWnd);}
	bool Refresh(HDC hDC,const RECT *rc);
	void CaretMove();
	void SetMarkVisible(int nMark){m_nMarkVisible = nMark;}

	/*キャレット位置から表示位置を調整する処理*/
	inline void SetFirstColumn(int nFirstColumn){m_nFirstVisibleColumn = nFirstColumn;}
	bool AdjustVisiblePos();
	bool AdjustVisibleLine();

//privateメンバ関数/////////////////////////////////////////////////////////
private:
	//! 選択領域描画のための初期化情報
	struct SelOldInfo{
		HPEN m_hPen,m_hPenOld;			//!< ペン
		HBRUSH m_hBrush,m_hBrushOld;	//!< ブラシ
		int m_nRopOld;					//!< ROPモード
	};

	static LRESULT CALLBACK FootyWinProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT MainProc(UINT msg,WPARAM wParam,LPARAM lParam);
#ifndef UNDER_CE
	static unsigned int GetWheelMsg();
#endif	/*not defined UNDER_CE*/
	
	/*外部ファイルを開く*/
	void StartApplication(const wchar_t *szFileName);
	
	/*バーの状態を設定する*/
	void SetBars();

	/*キーボード関連のメッセージ*/
	void OnImeComposition(LPARAM lParam);
	LRESULT OnImeRequest(WPARAM wParam,LPARAM lParam);
	void OnChar(wchar_t wc);
	bool OnKeyDown(int nKeyCode);
	
	/*フォーカス関連のメッセージ*/
	void OnGotFocus();
	void OnLostFocus();
	
	/*マウス関連のメッセージ*/
	void OnMouseDown(int x, int y);
	void OnMouseMove(int x, int y, bool bFromScrollTimer = false);
	void OnMouseUp(int x, int y);
	void OnMouseWheel(bool bIsForward);
	void OnMouseDouble(int x,int y);

	/*スクロール関係の処理*/
	void OnScrollTimer();
	void StopScrollTimer();
	void StartScrollTimer(bool bIsVertical,int nScrollNum);
	void OnVScroll(WPARAM wParam);
	void OnHScroll(WPARAM wParam);
	
	/*計算系のルーチン*/
	bool CalcInfoFromMouse(int x,int y,CEditPosition *pPos);
	bool CalcLineCount(int y,CEditPosition *pStart,CEditPosition *pEnd);
	enum RetOnUrl{
		ONURL_NONE,				/*何にもない*/
		ONURL_URL,				/*URL上*/
		ONURL_MAIL,				/*メール上*/
	};
	RetOnUrl IsOnUrl(CEditPosition *pPos,CUrlInfo *pInfo);
	
	// レンダリング関連の処理
	bool CaretRefresh();
	bool ScrollRefresh();
	bool LineChangedRefresh(LinePt pLine1, LinePt pLine2);
	inline bool LineChangedRefresh(){
		return LineChangedRefresh(
			m_pDocuments->GetCaretPosition()->GetLinePointer(),
			m_pDocuments->GetCaretPosition()->GetLinePointer()
		);
	}

	// レンダリング用内部ルーチン
	bool RenderLines(LinePt pStartLine,LinePt pEndLine);
	inline int GetTextPosX(int nColumn){return m_nLineCountWidth + 
		(nColumn - m_nFirstVisibleColumn) * (m_pFonts->GetWidth() + m_nWidthMargin);}
	static void DrawRectangle(HDC hDC,int x1,int y1,int x2,int y2,COLORREF color);
	static void DrawLine(HDC hDC,int x1,int y1,int x2,int y2,COLORREF color,int penType = PS_SOLID);
	bool RendLine(HDC hDC,LinePt pLine,int y,bool bEnd);
	bool InitDcForSelect(HDC hDC,SelOldInfo *pSelInfo);
	bool ReleaseDcForSelect(HDC hDC,SelOldInfo *pSelInfo);
	bool RendSelected(HDC hDC,CEthicLine *pLine,int y);
	bool RendLineCount(HDC hDC,const RECT *rc);
	bool RendBorderLine(HDC hDC,const RECT *rc);
	bool RendRuler(HDC hDC,const RECT *rc);
	bool RendUnderLine(HDC hDC,const RECT *rc);

//publicメンバ変数//////////////////////////////////////////////////////////
public:
	/*マージン*/
	int m_nWidthMargin;							//!< 文字の幅のマージン
	int m_nHeightMargin;						//!< 文字の高さマージン
	
	/*関数ポインタ*/
	Footy2FuncFocus m_pFuncFocus;				//!< フォーカス取得イベント
	void *m_pDataFocus;							//!< それのデータ

//privateメンバ変数/////////////////////////////////////////////////////////
private:
	enum enDragMode{							//!< マウスドラッグ状態
		DRAGMODE_NONE,							//!< ドラッグなし
		DRAGMODE_MOUSE,							//!< マウスドラッグ状態
		DRAGMODE_LINE,							//!< 行番号表示領域
		DRAGMODE_OLE,							//!< OLE DnD
	};
	enum enTimer{
		TIMER_AUTO_SCROLL = 1,					//!< オートスクロールタイマー
	};
	enum enFixedNumber{
		WHEEL_MOUSE = 3,						//!< ホイールマウスでスクロールする行数
		SCROLL_TIME = 150,						//!< スクロールタイマーの間隔(ms)
		PIXEL_PER_LINESPEED = 50,				//!< 自動スクロールの時の、一回当たり何回スクロールするかのピクセル数
		PIXEL_PER_COLUMNSPEED = 20,				//!< 自動スクロールの時の、一回当たり何回スクロールするかのピクセル数

		LINEICON_MARGIN = 1,					//!< 行番号アイコンのマージン
		ICON_HEIGHT = 16,						//!< 行番号アイコンの高さ
		ICON_WIDTH = 16,						//!< 行番号アイコンの幅
	};

	/*ウィンドウ情報*/
	int m_x,m_y;								//!< ウィンドウの位置
	int m_nWidth,m_nHeight;						//!< ウィンドウの大きさ
	HWND m_hWnd;								//!< ウィンドウハンドル
	HWND m_hOwner;								//!< オーナーハンドル
	int m_nRop2;								//!< ラスタオペレーション番号
	bool m_bIsFocused;							//!< フォーカスを持っている
	int m_nCaretWidth;							//!< キャレットの幅
	bool m_bVisible;							//!< 表示しているか？
	CDoubleBuffering m_cDoubleBuffering;		//!< ダブルバッファリング
	static HBITMAP m_bmpIcons[sizeof(int) * 8];	//!< 行アイコン

	/*システム情報*/
	int m_nFootyID;								//!< FootyのID
	int m_nViewID;								//!< ビューID
	CFontObjects *m_pFonts;						//!< フォント情報
	CFootyDoc *m_pDocuments;					//!< ドキュメント
	CFootyStatus *m_pStatus;					//!< ステータスポインタ
	CFootyView *m_pView;						//!< 他のビュー

	/*スクロールデータ*/
	size_t m_nVisibleLines;						//!< 表示可能な行数
	size_t m_nVisibleColumns;					//!< 表示可能な桁数
	SCROLLINFO m_vInfo;							//!< 垂直スクロールバーの情報
	SCROLLINFO m_hInfo;							//!< 水平スクロールバーの情報
#ifndef UNDER_CE
	static unsigned int  m_uWheelMsg;			//!< マウスホイールのメッセージ
#endif	/*not defined UNDER_CE*/
	bool m_bIsAutoScrolling;					//!< 自動スクロールの最中
	bool m_bIsAutoScrollVertical;				//!< 自動スクロールが上下か？
	int m_nAutoScrollNum;						//!< 自動スクロールの量

	/*各種大きさの情報*/
	int m_nRulerHeight;							//!< ルーラー高さ
	int m_nLineCountWidth;						//!< 行番号表示領域幅
	int m_nLineCountMargin;						//!< 行番号表示領域のマージン
	
	/*表示関連の情報*/
	CCaret m_cCaret;							//!< キャレット情報
	int m_nFirstVisibleColumn;					//!< 最初に表示されている桁
	int m_nNowMousePosX,m_nNowMousePosY;
	int m_nMarkVisible;							//!< エディタの描画情報
	
	/*一時的な変数*/
	unsigned long m_nIgnoreKey;					//!< 無視するキー
	bool m_bShiftLocked;						//!< シフトロックされているかどうか
	enDragMode m_nDragMode;						//!< ドラッグ状態
};

/*[EOF]*/
