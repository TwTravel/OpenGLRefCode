// matlightDoc.h : interface of the CMatlightDoc class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __MATDOC__
#define __MATDOC__

class CMatlightDoc : public CDocument
{
protected: // create from serialization only
	CMatlightDoc();
	DECLARE_DYNCREATE(CMatlightDoc)

// Attributes
public:
	GLfloat fLight0Pos[4];		// Light Position
	GLfloat fLight0Ambient[4];	// Lights Ambient Component
	GLfloat fLight0Diffuse[4];	// Lights Diffuse Component
	GLfloat fLight0Specular[4];	// Lights Secular Component
	
	GLfloat fMatAmbient[4];		// Materials Ambient reflectivity
	GLfloat fMatDiffuse[4];		// Materials Diffuse reflectivity
	GLfloat fMatSpecular[4];	// Materials Specular reflectivity
	GLfloat fShine;				// Materials Shininess

	GLint   nAlphaList;			// Display list for letters

// Operations
public:
	void GLSetupRC(HDC hDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMatlightDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMatlightDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMatlightDoc)
	afx_msg void OnEditLight();
	afx_msg void OnEditMaterial();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
