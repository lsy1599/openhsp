/**
 * @file FootyDoc.h
 * @brief ドキュメント管理クラスの実装
 * @author ShinjiWatanabe
 * @version 1.0
 */

#pragma once

#include "Footy2.h"
#include "EditPosition.h"
#include "UndoBuffer.h"
#include "EthicLine.h"
#include "RegExpContainer.h"


class CFootyDoc
{
public:
	// コンストラクタ
	CFootyDoc();
	~CFootyDoc();

	//! 新規作成
	void CreateNew(int nGlobalID);
	
	//! どの範囲を描画する必要があるのか
	enum RedrawType
	{
		REDRAW_FAILED,				//!< 関数失敗
		REDRAW_NONE,				//!< 再描画必要なし
		REDRAW_LINE,				//!< その行のみ再描画する
		REDRAW_CURMOVE,				//!< カーソル移動に伴う再描画のみ
		REDRAW_SCROLLED,			//!< スクロールされた
		REDRAW_SELCHANGED,			//!< 選択状態のみが変化した
		REDRAW_ALL,					//!< 全て再描画する必要がある
	};

	// データ処理
	void SetText(const wchar_t *pString);
	RedrawType InsertString(const wchar_t *pString,bool bRecUndo = true,bool bOverwritable = false,bool bMemLineMode = false);
	RedrawType InsertChar(wchar_t wChar);
	RedrawType InsertReturn();
	RedrawType InsertReturnUp();
	RedrawType InsertReturnDown();
	RedrawType Indent();
	RedrawType Unindent();
	RedrawType OnBackSpace();
	RedrawType OnDelete();

	// コマンド系
	bool Undo();
	bool Redo();
	RedrawType ClipPaste(HWND hWnd);
	bool ClipCopy(HWND hWnd);
	void SetSavePoint();

	// 移動系
	void MoveWordForward();						//!< 一つ単語をすっ飛ばして後に
	void MoveWordBack();						//!< 一つ単語をすっ飛ばして前に

	// 検索系
	bool Search(const wchar_t *szText, int nFlags);

	// 選択系
	bool SetSelectStart(CEditPosition *pStart);
	inline bool SetSelectStart(){return SetSelectStart(&m_cCaretPos);}
	bool SetSelectStart(CUndoBuffer::CPosInfo *pStart);
	bool SetSelectEndNormal(CEditPosition *pEnd);
	bool SetSelectEndNormal(CUndoBuffer::CPosInfo *pEnd);
	inline bool SetSelectEndNormal(){return SetSelectEndNormal(&m_cCaretPos);}
	bool SetSelectEndBox(CEditPosition *pEnd);
	inline void UnSelect(){m_nSelectType = SELECT_NONE;}
	void SelectWord();
	bool GetSelText(std::wstring *pString, LineMode nLineMode = LM_CRLF);
	inline void SetSelLineStart()
	{
		m_cLineSelStartA = *GetSelStart();
		m_cLineSelStartB = *GetSelEnd();
	}
	size_t GetSelLength(LineMode nLineMode = LM_CRLF);
	RedrawType DeleteSelected(CUndoBuffer *pUndoBuffer);
	void SelectAll();
	
	// データ取得
	LinePt GetLine(size_t nLine);
	inline LsLines *GetLineList(){return &m_lsLines;}
	inline LinePt GetTopLine(){return m_lsLines.begin();}	//!< 先頭行を取得する
	LinePt GetLastLine();									//!< 「本当の」最終行を取得する
	inline LinePt GetEndLine(){return m_lsLines.end();}		//!< 末尾のダミー行を取得する
	inline int GetLineMode(){return m_nLineMode;}
	inline LineSize GetLineNum(){return m_lsLines.size();}	//!< 行の数を取得する
	inline size_t GetLapelColumn(){return m_nLapelColumns;}
	inline int GetLapelMode(){return m_nLapelMode;}
	inline size_t GetTabLen(){return m_nTabLen;}
	bool IsEdited();
	inline bool IsInsertMode(){return m_bInsertMode;}

	// 場所系の取得ルーチン
	inline CEditPosition *GetCaretPosition(){return &m_cCaretPos;}
	inline bool IsSelecting(){return m_nSelectType != SELECT_NONE;}
	inline bool IsBoxSelect(){return m_nSelectType == SELECT_BOX || 
									 m_nSelectType == SELECT_BOX_REV;}
	inline CEditPosition *GetLineSelStartA(){return &m_cLineSelStartA;}
	inline CEditPosition *GetLineSelStartB(){return &m_cLineSelStartB;}
	inline CEthicLine *GetFirstVisible(int nID){return &m_cFirstVisibleLine[nID];}
	CEditPosition *GetSelStart();
	CEditPosition *GetSelEnd();
	
	// 設定
	void SetLapel(size_t nColumn,int nMode);
	void SetInsertMode(bool bInsertMode);

	// 強調表示系の関数
	bool AddEmphasis(const wchar_t *pString1,const wchar_t *pString2,int nType,
		int nFlags,int nLevel,int nOnLevel,int nIndependence,COLORREF nColor);
	void FlushEmphasis();
	void ClearEmphasis();
	void SetChacheCommand(LinePt pLine);

	// ドキュメント系のイベント呼び出し
	inline void SendMoveCaretCallBack()
	{
		if (m_pFuncMoveCaret)
			m_pFuncMoveCaret(m_nGlobalID,m_pDataMoveCaret,
							m_cCaretPos.GetLineNum(),m_cCaretPos.GetPosition());
	}
	inline void SendTextModified(int nCause)
	{
		if (m_pFuncTextModified)
			m_pFuncTextModified(m_nGlobalID,m_pDataTextModified,nCause);
	}

private:
	// サブルーチン
	void ReturnLine(LinePt *pNowLine,const wchar_t **pString,const wchar_t *pWork,
		size_t nPos,std::wstring *pRestStr,int n,bool bFirst);

	// 検索系
	void ParseRegulaerExpression(const wchar_t *szString,CRegExpContainer *pRegExp);
	bool SearchText(CEditPosition *pStart,CRegExpElement *pRegElement,bool bBackward = false);
	void SelectSearchedWord(LinePt pNowLine,size_t nPosition,CEditPosition *pEndPos);

	// 共通ルーチン
	bool PushBackUndo(CUndoBuffer *pUndo);
	void DeleteLine(LinePt pLine);
	void DeleteLine(LinePt pStart,LinePt pEnd);
	bool SetLineInfo(LinePt pBegin);
	bool SetLineInfo(LinePt pBegin,LinePt pEnd);

public:
	//! 選択タイプ
	enum SelType
	{
		SELECT_NONE,								//!< 選択していない
		SELECT_NORMAL,								//!< 通常の選択
		SELECT_REV,									//!< 上下逆の選択
		SELECT_BOX,									//!< 矩形選択
		SELECT_BOX_REV,								//!< 上下逆の矩形選択
	};
	
	// 状態
	bool m_bEnabled;								//!< 編集可能状態

	// 関数ポインタ
	Footy2FuncMoveCaret m_pFuncMoveCaret;			//!< キャレット移動イベント
	void *m_pDataMoveCaret;							//!< それのデータ
	Footy2FuncTextModified m_pFuncTextModified;		//!< テキストが編集されたときのイベント
	void *m_pDataTextModified;						//!< テキストが編集されたときのデータ
	Footy2FuncInsertModeChanged m_pFuncInsertMode;	//!< 挿入状態が変化されたときのイベント
	void *m_pDataInsertModeChanged;					//!< 挿入状態が変化されたときのデータ

private:
	// ドキュメントデータ
	LsLines m_lsLines;								//!< 文字列のデータ
	std::list<CUndoBuffer> m_lsUndoBuffer;			//!< アンドゥーバッファ
	std::list<CUndoBuffer>::iterator m_pNowUndoPos;	//!< 現在のアンドゥー位置
	LsWords m_lsEmphasisWord;						//!< 文字列色分け
	LineMode m_nLineMode;							//!< 改行コード
	bool m_bInsertMode;								//!< インサート状態

	// 場所・選択関係
	CEditPosition m_cCaretPos;						//!< 現在キャレットがある位置
	SelType m_nSelectType;							//!< 選択のタイプ
	CEditPosition m_cSelectBegin;					//!< 選択の開始位置
	CEditPosition m_cSelectEnd;						//!< 選択の終了位置
	CEditPosition m_cLineSelStartA;					//!< 行選択の開始位置(その場所の開始位置)
	CEditPosition m_cLineSelStartB;					//!< 行選択の開始位置(その場所の終了位置)
	CEthicLine m_cFirstVisibleLine[4];				//!< 最初に表示されている行

	// ビューデータ
	size_t m_nLapelColumns;							//!< 各ビューの折り返し桁数
	int m_nLapelMode;								//!< ビューモード
	size_t m_nTabLen;								//!< タブの横幅
	
	// 編集されていないかを調べるための変数
	std::list<CUndoBuffer>::iterator m_pSavedPos;	//!< セーブポイント
	bool m_bCannotReachSavedPos;					//!< セーブポイントまでたどり着けない(削除された)
	
	// そのほかデータ
	int m_nGlobalID;								//!< 親のID番号
};

/*[EOF]*/
