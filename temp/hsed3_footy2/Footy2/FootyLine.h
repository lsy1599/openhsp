/*===================================================================
CFootyLineクラス
Footyの行数を一行単位で管理するクラスです。
===================================================================*/

#pragma once

#include <vector>
#include <list>
#include "StaticVector.h"
#include "EmphasisWord.h"

class CUrlInfo{							/*URLごとの情報*/
public:
	CUrlInfo(){
		m_nStartPos = 0;
		m_nEndPos = 0;
	}
	
	/*ポジションが含まれるかチェックする*/
	inline bool IsInPosition(size_t nPosition){
		return m_nStartPos <= nPosition &&
			nPosition < m_nEndPos;
	}
	inline size_t GetLength(){return m_nEndPos - m_nStartPos;}

public:
	size_t m_nStartPos;					/*URLの開始位置0ベース*/
	size_t m_nEndPos;					/*URLの終了位置0ベース*/
};

typedef std::vector<CUrlInfo>::iterator UrlIterator;

class CFootyLine{						/*行ごとの情報*/
public:
	/*構造体を定義する*/
	struct EmpPos{						/*GUI部分に送るコマンド構造体*/
		size_t m_nPosition;				/*コマンドの位置*/
		bool m_bIsStart;				/*trueのとき、ここが色を変えるポイント*/
		COLORREF m_Color;				/*設定する色*/
	};
	struct EthicInfo{					/*論理計測ルーチンの戻り*/
		size_t m_nEthicLine;			/*論理行*/
		size_t m_nEthicColumn;			/*論理桁位置*/
	};
	struct WordInfo{					/*単語の情報*/
		size_t m_nBeginPos;				/*開始位置(0ベース)*/
		size_t m_nEndPos;				/*終了位置(0ベース)*/
	};
	
	/*コンストラクタ*/
	CFootyLine();
	
	/*文字列操作終了後に呼び出す*/
	typedef CStaticVector<WordPt,sizeof(int)*8> TakeOver;
	bool SearchEmphasis(TakeOver *pBeforeBetween,LsWords *plsWords);
	bool FlushString(size_t nTabLen,size_t nColumn,int nMode);
	bool SetPrevLineInfo(std::list<CFootyLine>::iterator pPrevLine);
	inline void SetEmphasisChached(bool bChached){m_bEmphasisChached = bChached;}

	/*取得ルーチン*/
	inline const wchar_t *GetLineData(){return &m_strLineData[0];}
	inline size_t GetLineLength(){return m_strLineData.size();}
	inline size_t GetEthicLine(){return m_nEthicLine;}
	inline size_t GetOffset(){return m_nLineOffset;}
	inline size_t GetRealLineNum(){return m_nRealLineNum;}
	inline CStaticVector<WordPt,sizeof(int)*8> *GetBetweenNext()
		{return &m_vecLineBetweenAfter;}
	inline std::vector<EmpPos> *GetColorInfo(){return &m_vecColorInfo;}
	inline std::vector<CUrlInfo> *GetUrlInfo(){return &m_vecUrlInfo;}
	inline std::vector<CUrlInfo> *GetMailInfo(){return &m_vecMailInfo;}
	inline bool EmphasisChached(){return m_bEmphasisChached;}
	inline int GetLineIcons(){return m_nLineIcons;}
	inline void SetLineIcons(int nLineIcons){m_nLineIcons = nLineIcons;}

	/*文字情報を元に計算を行うルーチン*/
	EthicInfo CalcEthicLine(size_t nPos,size_t nColumn,size_t nTab,int nMode);
	size_t CalcRealPosition(size_t nEthicLine,size_t nEthicPos,size_t nColumn,size_t nTab,int nMode);
	bool IsGoNext(const wchar_t *pChar,size_t nPos,size_t nNowCol,size_t nColumns,int nMode);
	WordInfo GetWordInfo(size_t nPos,bool bIsBackMode = true);

	/*文字関連*/
	enum CharSets{
		CHARSETS_ANSI_ALPHABET,				//!< ANSI(アルファベット)
		CHARSETS_ANSI_NUMBER,				//!< ANSI(数字)
		CHARSETS_ANSI_SYMBOL,				//!< ANSI(アンダーバーを除く記号)
		CHARSETS_ANSI_UNDER,				//!< ANSIアンダーバー)
		CHARSETS_HIRAGANA,					//!< 日本語(ひらがな)
		CHARSETS_KATAKANA,					//!< 日本語(カタカナ)
		CHARSETS_KATAKANA_HALF,				//!< 日本語(半角カタカナ)
		CHARSETS_KANJI,						//!< 日本語(漢字)
		CHARSETS_FULL_ALPHABET,				//!< 全角アルファベット
		CHARSETS_FULL_NUMBER,				//!< 全角数字
		CHARSETS_AINU_EXTENSION,			//!< アイヌ語用拡張
		CHARSETS_IPA_EXTENSION,				//!< IPA拡張
		CHARSETS_SYMBOLS,					//!< 記号
		CHARSETS_JP_NOBASHI,				//!< 日本語のばし音
		CHARSETS_KOREA,						//!< 韓国語
		CHARSETS_LATIN,						//!< ラテン
		CHARSETS_BALTIC,					//!< バルト諸国
		CHARSETS_ARABIC,					//!< アラビア
		CHARSETS_HEBREW,					//!< ヘブライ
		CHARSETS_GREEK,						//!< ギリシャ
		CHARSETS_CYRILLIC,					//!< キリル文字
		CHARSETS_VIETTNAMESE,				//!< ベトナム語
		CHARSETS_THAI,						//!< タイ語
		
		CHARSETS_UNKNOWN,					//!< 不明
	};

	static CharSets GetCharSets(wchar_t w);
	static CharSets GetCharSets(wchar_t w1,wchar_t w2);
	static bool IsDualChar(CharSets c);		/*二倍の横幅の文字か取得する*/
	static inline bool IsDualChar(wchar_t w){return IsDualChar(GetCharSets(w));}
	static inline bool IsDualChar(wchar_t w1,wchar_t w2){return IsDualChar(GetCharSets(w1,w2));}
	static inline bool IsSurrogateLead(wchar_t w){return 0xD800 <= w && w <= 0xDBFF;}
	static inline bool IsSurrogateTail(wchar_t w){return 0xDC00 <= w && w <= 0xDFFF;}

private:
	/*内部ルーチン*/
	bool IsMatched(const wchar_t *pStr1,const wchar_t *pStr2,size_t nLen);
	
	/*クリッカブル関連の処理*/
	bool FindMail(size_t start,size_t *pBegin,size_t *pEnd);
	bool FindURL(size_t start,size_t *pBegin,size_t *pEnd);
	bool IsMailChar(wchar_t wc);
	bool IsURLChar(wchar_t wc);

public:
	std::wstring m_strLineData;				//!< 一行のデータ

private:
	std::vector<EmpPos> m_vecColorInfo;		//!< 色分け情報
	TakeOver m_vecLineBetweenAfter;			//!< この後に引き継ぐ複数行強調表示文字列
	std::vector<CUrlInfo> m_vecUrlInfo;		//!< URL情報
	std::vector<CUrlInfo> m_vecMailInfo;	//!< メール情報
	size_t m_nEthicLine;					//!< 論理何行分？
	size_t m_nLineOffset;					//!< 論理行オフセット位置
	size_t m_nRealLineNum;					//!< 本当の行番号
	bool m_bEmphasisChached;				//!< 強調文字列がキャッシュされている
	int m_nLineIcons;						//!< 行ごとのアイコン
};

/*長いタイプの宣言*/
typedef std::list<CFootyLine>::iterator LinePt;
typedef std::list<CFootyLine>::size_type LineSize;
typedef std::list<CFootyLine> LsLines;

/*[EOF]*/
