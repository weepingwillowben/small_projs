
#include "Player.h"
class RealPlayer :
    public Player
{
public:
    RealPlayer(void);
    virtual ~RealPlayer(void);
    SelectTypes SelectedThing;
    Point Selected1;
    Point Selected2;
    int NumOfBuyTroops;
    void DrawInterface();
    void RightClick(int xbox,int ybox);
    void LeftClick(int xbox,int ybox);
    bool BuyAttach(int TroopNum);
    void PlaceWall(Point Start, Point End);
    void SelectThing(SelectTypes Thing);
    bool ChangeTurn();
    bool TroopBuyPutter(Building * B, Point & OutSquare, Point GetCloseTo);
};
