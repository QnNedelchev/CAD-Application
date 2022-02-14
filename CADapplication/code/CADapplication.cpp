#include <iostream>
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h";


#define HORIZONTAL = 0;
#define VERTICAL = 1;

//=====================================================DEFINING THE BASE STRUCTURS NEEDED===========================================================

struct sShape;
struct nMeasure;

struct sNode
{
	sShape* parent;
	olc::vf2d pos;
};

struct sShape
{
	std::vector<sNode> vecNodes;
	uint32_t maxNodes = 0;
	olc::Pixel col = olc::GREEN;

	olc::vf2d vStartPos = {0.0f , 0.0f};
	olc::vf2d vEndPos = {0.0f , 0.0f};
	olc::vf2d vMeasurePos = { 0.0f , 0.0f };
	nMeasure* measureNote = nullptr;
	sShape* parent;

	bool isMeasure;

	static float fWorldScale;
	static olc::vf2d vWorldOffset;

	void WorldToScreen(const olc::vf2d& v, int& nScreenX, int& nScreenY)
	{
		nScreenX = (int)((v.x - vWorldOffset.x) * fWorldScale);
		nScreenY = (int)((v.y - vWorldOffset.y) * fWorldScale);
	}

	virtual void DrawSelf(olc::PixelGameEngine* pge) = 0;

	sNode* GetNextNode(const olc::vf2d& p)
	{
		if (vecNodes.size() == maxNodes)
			return nullptr; //If we reached the max nodes the shape can have, we return null pointer,we don't need to look for new node

		sNode newNode;
		newNode.parent = this;
		newNode.pos = p;
		vecNodes.push_back(newNode);

		//Bad practice is to return a pointer to an element in a vector/array/list etc. 
		//But if you restrict the vector it won't be a problem. This is what's been done in each shape's contructor
		//by reserving space in the vector equal to the maximum number of nodes that the shape can have
		return &vecNodes[vecNodes.size() - 1]; 
	}

	sNode* HitNode(olc::vf2d& p)
	{
		for (auto& n : vecNodes)
		{
			if ((p - n.pos).mag() < 0.01f)
				return &n;
		}

		return nullptr;
	}

	virtual void DrawNodes(olc::PixelGameEngine* pge)
	{
		for (auto& n : vecNodes)
		{
			int sx, sy;
			WorldToScreen(n.pos, sx, sy);
			pge->FillCircle(sx, sy, 2, olc::RED);
		}
	}
};

//===============================================================================================================================================================

//Initialize the static members of the SHAPE struct
float sShape::fWorldScale = 1.0f;
olc::vf2d sShape::vWorldOffset = { 0.0f,0.0f };

//=============================================================================SHAPES============================================================================

struct sLine : public sShape
{
	sLine()
	{
		maxNodes = 2;
		vecNodes.reserve(maxNodes);
		isMeasure = false;
		parent = nullptr;
	}

	void DrawSelf(olc::PixelGameEngine* pge) override
	{
		int sx, sy, ex, ey;
		WorldToScreen(vecNodes[0].pos, sx, sy);
		WorldToScreen(vecNodes[1].pos, ex, ey);
		pge->DrawLine(sx, sy, ex, ey, col);
	}
};

struct sDashLine : public sShape
{
	sDashLine()
	{
		maxNodes = 2;
		vecNodes.reserve(maxNodes);
		isMeasure = false;
		parent = nullptr;
	}

	void DrawSelf(olc::PixelGameEngine* pge) override
	{
		int sx, sy, ex, ey;
		WorldToScreen(vecNodes[0].pos, sx, sy);
		WorldToScreen(vecNodes[1].pos, ex, ey);
		pge->DrawLine(sx, sy, ex, ey, col, 0xF0F0F0F0);
	}
};

struct sBox : public sShape
{
	sBox()
	{
		maxNodes = 2;
		vecNodes.reserve(maxNodes);
		isMeasure = false;
		parent = nullptr;
	}

	void DrawSelf(olc::PixelGameEngine* pge) override
	{
		int sx, sy, ex, ey;
		WorldToScreen(vecNodes[0].pos, sx, sy);
		WorldToScreen(vecNodes[1].pos, ex, ey);
		pge->DrawRect(sx, sy, ex - sx, ey - sy, col);
	}
};

struct sCircle : public sShape
{
	sCircle()
	{
		maxNodes = 2;
		vecNodes.reserve(maxNodes);
		isMeasure = false;
		parent = nullptr;
	}

	void DrawSelf(olc::PixelGameEngine* pge) override
	{
		float radius = (vecNodes[0].pos - vecNodes[1].pos).mag();
		int sx, sy, ex, ey;
		WorldToScreen(vecNodes[0].pos, sx, sy);
		WorldToScreen(vecNodes[1].pos, ex, ey);
		pge->DrawLine(sx, sy, ex, ey, olc::DARK_GREY, 0xF0F0F0F0);
		pge->DrawCircle(sx, sy, radius * fWorldScale, col);
	}
};

struct sCurve : public sShape
{
	sCurve()
	{
		maxNodes = 3;
		vecNodes.reserve(maxNodes);
		isMeasure = false;
		parent = nullptr;
	}

	void DrawSelf(olc::PixelGameEngine* pge) override
	{
		int sx, sy, ex, ey;

		if (vecNodes.size() < 3)
		{
			//Draw 1ft to second node
			WorldToScreen(vecNodes[0].pos, sx, sy);
			WorldToScreen(vecNodes[1].pos, ex, ey);
			pge->DrawLine(sx, sy, ex, ey, olc::DARK_GREY, 0xF0F0F0F0);
		}
		
		if (vecNodes.size() == 3)
		{
			//Draw first to second node 
			WorldToScreen(vecNodes[0].pos, sx, sy);
			WorldToScreen(vecNodes[1].pos, ex, ey);
			pge->DrawLine(sx, sy, ex, ey, olc::DARK_GREY, 0xF0F0F0F0);

			//Draw second structural line
			WorldToScreen(vecNodes[1].pos, sx, sy);
			WorldToScreen(vecNodes[2].pos, ex, ey);
			pge->DrawLine(sx, sy, ex, ey, olc::DARK_GREY, 0xF0F0F0F0);

			olc::vf2d op = vecNodes[0].pos;
			olc::vf2d np = op;
			for (float t = 0; t < 1.0f; t += 0.01f)
			{
				np = (1 - t) * (1 - t) * vecNodes[0].pos + 2 * (1 - t) * t * vecNodes[1].pos + t * t * vecNodes[2].pos;
				WorldToScreen(op, sx, sy);
				WorldToScreen(np, ex, ey);
				pge->DrawLine(sx, sy, ex, ey, col);
				op = np;
			}
		}
	}
};

struct sTriangle : public sShape
{
	sTriangle()
	{
		maxNodes = 3;
		vecNodes.reserve(maxNodes);
		isMeasure = false;
		parent = nullptr;
	}

	void DrawSelf(olc::PixelGameEngine* pge) override
	{
		int sx, sy, ex, ey;

		if (vecNodes.size() < 3)
		{
			//Draw 1st to second node
			WorldToScreen(vecNodes[0].pos, sx, sy);
			WorldToScreen(vecNodes[1].pos, ex, ey);
			pge->DrawLine(sx, sy, ex, ey, col, 0xF0F0F0F0);
		}

		if (vecNodes.size() == 3)
		{
			//Draw first to second node
			WorldToScreen(vecNodes[0].pos, sx, sy);
			WorldToScreen(vecNodes[1].pos, ex, ey);
			pge->DrawLine(sx, sy, ex, ey, col);

			//Draw second side of the triangle
			WorldToScreen(vecNodes[1].pos, sx, sy);
			WorldToScreen(vecNodes[2].pos, ex, ey);
			pge->DrawLine(sx, sy, ex, ey, col);

			//Draw the 3rd side of the triangle
			WorldToScreen(vecNodes[2].pos, sx, sy);
			WorldToScreen(vecNodes[0].pos, ex, ey);
			pge->DrawLine(sx, sy, ex, ey, col);
		}
	}
};

struct sMeasure : public sShape
{
	sMeasure()
	{
		maxNodes = 2;
		vecNodes.reserve(maxNodes);
		isMeasure = true;
		parent = nullptr;
	}

	bool bFinished = false;
	olc::Pixel col = olc::DARK_GREY;

	void DrawSelf(olc::PixelGameEngine* pge) override
	{
		int sx, sy, ex, ey;
		WorldToScreen(vecNodes[0].pos, sx, sy);
		WorldToScreen(vecNodes[1].pos, ex, ey);
		pge->DrawLine(sx, sy, ex, ey, col);
	}

	virtual void DrawNodes(olc::PixelGameEngine* pge) override
	{
		for (auto& n : vecNodes)
		{
			int sx, sy;
			WorldToScreen(n.pos, sx, sy);
			pge->FillCircle(sx, sy, 2, olc::GREY);
		}
	}
};

struct nMeasure 
{
	sShape* parent;
	olc::vf2d pos;
	float value;
};

//===============================================================================================================================================================


//=========================================================================HELPER FUNCTIONS======================================================================


void FindLineLenght(olc::vf2d vStartNodePos, olc::vf2d vEndNodePos, float &d)
{
	d = std::sqrtf((vEndNodePos.x - vStartNodePos.x) * (vEndNodePos.x - vStartNodePos.x) + (vEndNodePos.y - vStartNodePos.y) * (vEndNodePos.y - vStartNodePos.y));
}

//=========================================================================APPLICATION CLASS=====================================================================

class MyCAD : public olc::PixelGameEngine
{
public:
	MyCAD()
	{
		sAppName = "CAD";
	}

private:
	olc::vf2d vOffset = { 0.0f,0.0f };
	olc::vf2d vStartPan = { 0.0f,0.0f };
	olc::vf2d vCursor = { 0.0f,0.0f };

	olc::vf2d vStartNodePos = { 0.0f,0.0f };
	olc::vf2d vEndNodePos = { 0.0f,0.0f };

	float fLenght = 0.0f;
	float fScale = 10.0f;
	float fGrid = 1.0f;

	sShape* tempShape = nullptr;
	std::list<sShape*> listShapes;
	sNode* selectedNode = nullptr;

	std::list<nMeasure*> listMeasures;

	bool bSnapToGrid = true;
	bool bMeasure = false;
	bool bEditMode = false;
	bool bMeasureMode = false;
	bool bDrawMode = true;
	bool bLine = false;
	bool bToolBox = true;
	bool panning = false;

	int nMeasurePoints = 0;

	std::string currentMode = "DRAW";
	std::string currentShape = "";

	void WorldToScreen(const olc::vf2d& v, int& nScreenX, int& nScreenY)
	{
		nScreenX = (int)((v.x - vOffset.x) * fScale);
		nScreenY = (int)((v.y - vOffset.y) * fScale);
	}

	void ScreenToWorld(int nScreenX, int nScreenY, olc::vf2d& v)
	{
		v.x = (float)(nScreenX) / fScale + vOffset.x;
		v.y = (float)(nScreenY) / fScale + vOffset.y;
	}

public:

	bool OnUserCreate() override
	{
		vOffset = { (float)(-ScreenWidth() / 2) / fScale, (float)(-ScreenHeight() / 2) / fScale };
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//==================================================================PANNING & ZOOMING====================================================================

		olc::vf2d vMouse = { (float)GetMouseX(), (float)GetMouseY() };

		//Panning input
		if (GetKey(olc::Key::CTRL).bPressed)
		{
			vStartPan = vMouse;
			panning = true;
		}

		if (GetKey(olc::Key::CTRL).bHeld)
		{
			vOffset -= (vMouse - vStartPan) / fScale;
			vStartPan = vMouse;
		}

		if (GetKey(olc::Key::CTRL).bReleased)
		{
			for (auto& measure : listMeasures)
			{
				olc::vf2d pos;
				int x, y;

				//Calculate the measure note position
				WorldToScreen(measure->parent->vMeasurePos, x, y);
				measure->pos = { (float)(x - x / 8), (float)(y + y / 10) };
			}
			panning = false;
		}

		olc::vf2d vMouseBeforeZoom;
		ScreenToWorld((int)vMouse.x, (int)vMouse.y, vMouseBeforeZoom);

		//Zoom In
		if (GetKey(olc::Key::Z).bHeld || GetMouseWheel() > 0)
		{
			fScale *= 1.1f;
		}

		//Zoom Out
		if (GetKey(olc::Key::X).bHeld || GetMouseWheel() < 0)
		{
			fScale /= 1.1f;
		}

		//Enable/Disable Grid Snap
		if (GetKey(olc::Key::G).bReleased)
		{
			bSnapToGrid = !bSnapToGrid;
		}

		olc::vf2d vMouseAfterZoom;
		ScreenToWorld((int)vMouse.x, (int)vMouse.y, vMouseAfterZoom);
		vOffset += (vMouseBeforeZoom - vMouseAfterZoom);

		if (bSnapToGrid == true)
		{
			vCursor.x = floorf((vMouseAfterZoom.x + 0.5f) * fGrid);
			vCursor.y = floorf((vMouseAfterZoom.y + 0.5f) * fGrid);
		}
		else
		{
			vCursor.x = vMouseAfterZoom.x;
			vCursor.y = vMouseAfterZoom.y;
		}

		//=======================================================================================================================================================

		//=====================================================================MODE SELECTION====================================================================

		//EDIT MODE
		if (GetKey(olc::Key::E).bPressed)
		{
			bEditMode = true;
			bMeasureMode = false;
			bDrawMode = false;
			bMeasure = false;
		}

		//MEASURE MODE
		if (GetKey(olc::Key::M).bPressed)
		{
			bMeasureMode = true;
			bEditMode = false;
			bDrawMode = false;
		}

		//DRAW MODE
		if (GetKey(olc::Key::D).bPressed)
		{
			bDrawMode = true;
			bMeasureMode = false;
			bEditMode = false;
		}

		//============================================================================================================================================================


		//=====================================================================HANDLING SHAPES===================================================================

		if (bDrawMode)
		{
			currentMode = "DRAW";

			//LINE
			if (GetKey(olc::Key::L).bPressed)
			{
				currentShape = "LINE";
				tempShape = new sLine();
				bLine = true;
				//Place the first node of the line at the position of the mouse cursor
				selectedNode = tempShape->GetNextNode(vCursor);
				vStartNodePos = vCursor;

				//Get the next node that will be manipulated
				selectedNode = tempShape->GetNextNode(vCursor);
				vEndNodePos = vCursor;
			}

			//DASH LINE
			if (GetKey(olc::Key::A).bPressed)
			{
				currentShape = "AXIS LINE";
				tempShape = new sDashLine();
				bLine = false;

				//Place the first node of the line at the position of the mouse cursor
				selectedNode = tempShape->GetNextNode(vCursor);
				vStartNodePos = vCursor;

				//Get the next node that will be manipulated
				selectedNode = tempShape->GetNextNode(vCursor);
				vEndNodePos = vCursor;
			}

			//BOX
			if (GetKey(olc::Key::B).bPressed)
			{
				currentShape = "BOX";
				tempShape = new sBox();
				bLine = false;
				//Place the first node of the line at the position of the mouse cursor
				selectedNode = tempShape->GetNextNode(vCursor);

				//Get the next node that will be manipulated
				selectedNode = tempShape->GetNextNode(vCursor);
			}

			//CIRCLE
			if (GetKey(olc::Key::C).bPressed)
			{
				currentShape = "CIRCLE";
				tempShape = new sCircle();
				bLine = false;
				//Place the first node of the line at the position of the mouse cursor
				selectedNode = tempShape->GetNextNode(vCursor);

				//Get the next node that will be manipulated
				selectedNode = tempShape->GetNextNode(vCursor);
			}

			//CURVE/SPLINE
			if (GetKey(olc::Key::S).bPressed)
			{
				currentShape = "SPLINE";
				tempShape = new sCurve();
				bLine = false;
				//Place the first node of the line at the position of the mouse cursor
				selectedNode = tempShape->GetNextNode(vCursor);

				//Get the next node that will be manipulated
				selectedNode = tempShape->GetNextNode(vCursor);
			}

			//TRIANGLE
			if (GetKey(olc::Key::T).bPressed)
			{
				currentShape = "TRIANGLE";
				tempShape = new sTriangle();
				bLine = false;
				//Place the first node of the line at the position of the mouse cursor
				selectedNode = tempShape->GetNextNode(vCursor);

				//Get the next node that will be manipulated
				selectedNode = tempShape->GetNextNode(vCursor);
			}
		
		}
	
		//===================================================================================================================================================


		//==================================================================HANDLE TOOLBOX===================================================================

		if (GetKey(olc::Key::SHIFT).bPressed)
		{
			bToolBox = !bToolBox;
		}


		//EDIT SHAPE
		if (bEditMode)
		{
			currentShape = "";
			currentMode = "EDIT";

			if (GetMouse(0).bPressed)
			{
				selectedNode = nullptr;
				for (auto& shape : listShapes)
				{
					selectedNode = shape->HitNode(vCursor);
					if (selectedNode != nullptr)
					{
						selectedNode->parent->col = olc::GREEN;
						break;
					}
				}
			}

			if (GetMouse(1).bPressed)
			{
				selectedNode = nullptr;
				for (auto& shape : listShapes)
				{
					selectedNode = shape->HitNode(vCursor);
					if (selectedNode != nullptr)
					{
						listMeasures.remove(selectedNode->parent->measureNote);
						listShapes.remove(selectedNode->parent);
						tempShape = nullptr;
						break;
					}
				}
			}
		}

		//MEASURE LENGHT OF A LINE
		if (bMeasureMode)
		{
			currentMode = "MEASURE";
			nMeasure* note = new nMeasure();
		
			if (GetMouse(1).bPressed && !bMeasure)
			{
				
				currentShape = "";
				tempShape = new sMeasure();
				tempShape->isMeasure = true;
				olc::vf2d offset = { 0.0f, 4.0f };

				//Place the first node of the line at the position of the mouse cursor
				selectedNode = tempShape->GetNextNode(vCursor + offset);
				vStartNodePos = vCursor;
				tempShape->parent = selectedNode->parent;

				//Get the next node that will be manipulated
				selectedNode = tempShape->GetNextNode(vCursor + offset);
				vEndNodePos = vCursor;	
			}

			if (GetMouse(0).bReleased)
			{
				if (tempShape != nullptr)
				{
					selectedNode = tempShape->GetNextNode(vCursor);
					
					//If the next node is null pointer, then the shape is completed
					if (selectedNode == nullptr)
					{
						note->parent = tempShape;
						note->parent->measureNote = note;
						note->parent->parent = tempShape->parent;
						olc::vf2d pos;
						int x, y;

						//Calculate the measure note position

						WorldToScreen(vEndNodePos, x, y);
						note->parent->vMeasurePos = vEndNodePos;

						note->pos = {(float)(x - x/8), (float)(y + y/10)};

						tempShape->col = olc::WHITE;
						listShapes.push_back(tempShape);
						FindLineLenght(vStartNodePos, vEndNodePos, fLenght);

						note->value = fLenght;
						listMeasures.push_back(note);
					}
				}
			}

			/*if (GetMouse(0).bReleased)
			{
				selectedNode = nullptr;
				for (auto& shape : listShapes)
				{
					selectedNode = shape->HitNode(vCursor);
					if (selectedNode != nullptr)
					{
						if (bMeasure != true)
						{
							vStartNodePos = selectedNode->pos;
							vEndNodePos = { 0.0f,0.0f };
							bMeasure = true;
							nMeasurePoints = 1;
							break;
						}
						else
						{
							vEndNodePos = selectedNode->pos;
							bMeasure = false;
							selectedNode = nullptr;
							nMeasurePoints = 2;

							FindLineLenght(vStartNodePos, vEndNodePos, fLenght);
							//note->pos = { (vEndNodePos.x - vStartNodePos.x) / 2, (vEndNodePos.y - vStartNodePos.y) / 2 };
							note->value = fLenght;
							listMeasures.push_back(note);
							break;
						}
					}
				}
			}*/
			
			

		}

		if (selectedNode != nullptr)
		{
			if (bMeasure == false)
			{
				selectedNode->pos = vCursor;
				vEndNodePos = vCursor;
			}		
		}

		if (GetMouse(0).bReleased)
		{
			if (tempShape != nullptr)
			{
				selectedNode = tempShape->GetNextNode(vCursor);

				//If the next node is null pointer, then the shape is completed
				if (selectedNode == nullptr)
				{
					tempShape->col = olc::WHITE;
					listShapes.push_back(tempShape);
				}
			}

		}

		//Find the lenght of the line using the Pythagorean Theorem
		FindLineLenght(vStartNodePos, vEndNodePos, fLenght);


		//=========================================================================DRAWING=======================================================================

		//Clear screen
		Clear(olc::VERY_DARK_BLUE);

		int sx, sy;
		int ex, ey;

		//Get visible world
		olc::vf2d vWorldTopLeft, vWorldBottomRight;
		ScreenToWorld(0, 0, vWorldTopLeft);
		ScreenToWorld(ScreenWidth(), ScreenHeight(), vWorldBottomRight);

		vWorldTopLeft.x = floor(vWorldTopLeft.x);
		vWorldTopLeft.y = floor(vWorldTopLeft.y);
		vWorldBottomRight.x = ceil(vWorldBottomRight.x);
		vWorldBottomRight.y = ceil(vWorldBottomRight.y);

		//Draw GRID dots
		for (float x = vWorldTopLeft.x; x < vWorldBottomRight.x; x += fGrid)
		{
			for (float y = vWorldTopLeft.y; y < vWorldBottomRight.y; y += fGrid)
			{
				WorldToScreen({ x,y }, sx, sy);
				Draw(sx, sy, olc::BLUE);
			}
		}

		//Draw axis
		WorldToScreen({ 0,vWorldTopLeft.y }, sx, sy);
		WorldToScreen({ 0,vWorldBottomRight.y }, ex, ey);
		DrawLine(sx, sy, ex, ey, olc::GREY, 0xF0F0F0F0);
		WorldToScreen({vWorldTopLeft.x, 0}, sx, sy);
		WorldToScreen({vWorldBottomRight.x, 0}, ex, ey);
		DrawLine(sx, sy, ex, ey, olc::GREY, 0xF0F0F0F0);


		//======================================================================Draw shapes======================================================================

		//Update the static members of the shape
		sShape::fWorldScale = fScale;
		sShape::vWorldOffset = vOffset;

		for (auto& shape : listShapes)
		{
			if (!bEditMode)
			{
				shape->col = olc::WHITE;
			}
			shape->DrawSelf(this);
			shape->DrawNodes(this);
			
			if (shape->isMeasure)
			{
				DrawLine(shape->parent->vStartPos.x, shape->parent->vStartPos.y, shape->vStartPos.x, shape->vStartPos.y, olc::GREY, 0xF0F0F0F0);
				DrawLine(shape->parent->vEndPos.x, shape->parent->vEndPos.y, shape->vEndPos.x, shape->vEndPos.y, olc::GREY, 0xF0F0F0F0);
			}
		}

		if (!panning)
		{
			for (auto& measure : listMeasures)
			{
				DrawString(measure->pos.x, measure->pos.y, "SIZE: " + std::to_string(measure->value), olc::RED, 1);
			}
		}
		
	
		if (tempShape != nullptr)
		{
			tempShape->DrawSelf(this);
			tempShape->DrawNodes(this);
		}

		//=======================================================================================================================================================

		//Draw cursor
		WorldToScreen(vCursor, sx, sy);
		DrawCircle(sx, sy, 3, olc::YELLOW);


		//------------------------------------HEADER-------------------------------------------------------

		//Draw cursor coordinates
		DrawString(10, 10, "X: " + std::to_string(vCursor.x) + ", Y: " + std::to_string(vCursor.y), olc::YELLOW, 1);

		//Draw Line segment size
		if (fLenght > 0 && nMeasurePoints != 1)
		{
			DrawString(250, 10, "SIZE:" + std::to_string(fLenght), olc::YELLOW, 1);
		}

		//Draw current mode
		DrawString(1100, 10, "MODE: " + currentMode, olc::YELLOW, 2);

		//Draw current shape
		DrawString(700, 10, "SHAPE: " + currentShape, olc::YELLOW, 2);


		//-------------------------------------TOOLBOX------------------------------------------------------

		//Draw TOOLBOX title
		if (bToolBox)
			DrawString(8, 60, "TOOLBOX", olc::YELLOW, 2);
		else
			DrawString(8, 60, "TOOLBOX", olc::VERY_DARK_YELLOW, 2);

		//Draw TOOLBOX button
		if (bToolBox)
			FillCircle(128, 70, 5, olc::RED);
		else
			DrawCircle(128, 70, 5, olc::RED);

		if (bToolBox)
		{
			//Draw TOOLBOX area
			FillRect(8, 80, 112, 268, olc::YELLOW); //Border
			FillRect(12, 84, 104, 260, olc::BLACK);

			//Draw TOOLBOX items
			DrawString(36, 94, "SHAPES", olc::RED, 1.9);

			DrawString(16, 118, "B: Box", olc::WHITE, 1.6);
			DrawString(16, 132, "C: Circle", olc::WHITE, 1.6);
			DrawString(16, 146, "L: Line", olc::WHITE, 1.6);
			DrawString(16, 160, "S: Spline", olc::WHITE, 1.6);
			DrawString(16, 174, "A: Axis Line", olc::WHITE, 1.6);
			DrawString(16, 188, "T: Triangle", olc::WHITE, 1.6);

			DrawString(36, 218, "MODES", olc::RED, 1.9);

			DrawString(16, 242, "D: Draw", olc::WHITE, 1.6);
			DrawString(16, 256, "E: Edit", olc::WHITE, 1.6);
			DrawString(16, 270, "M: Measure", olc::WHITE, 1.6);
		}
		
		//=======================================================================================================================================================

		return true;
	}
};


//===============================================================================================================================================================


int main()
{
	MyCAD application;
	if (application.Construct(1600, 840, 1, 1))
		application.Start();

	return 0;
}
