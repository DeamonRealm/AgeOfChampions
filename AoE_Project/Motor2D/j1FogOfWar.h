#ifndef _FOG_OF_WAR_
#define _FOG_OF_WAR_

#include "j1Module.h"
#include "Iso_Primitives.h"
#include "Quadtree.h"
#include "j1Timer.h"


#define ALPHA_LIMIT 255			/* Values between 0 & 255 */
#define MID_ALPHA 150			/* Values between 0 & 255 */
#define	DIVISIONS_PER_PIXELS 40	/* Values between 20 & 60 in debug for >60 fps */ /* Values between 20 & 130 in release for >60 fps */
#define RENDER_MARGIN 80		/* Values between 0 & INF */
#define	UPDATE_TIME	20			/* Time in MS that the fog have to do the update */
#define UPDATE_RATE 500			/* Time in MS between the fog update call */

class Unit;
class Entity;

enum FOG_TYPE
{
	NO_FOG = 0,
	GRAY_FOG,
	DARK_FOG
};

class j1FogOfWar : public j1Module
{
public:

	j1FogOfWar();
	~j1FogOfWar();

public:

	void Init();

	// Called each loop iteration
	bool PostUpdate();

	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;

private:

	FOG_TYPE*				fog_layer = nullptr;	/*Layer that contains fog types*/
	AlphaCell*				alpha_layer = nullptr;	/*Layer that contains fog alpha values*/
	QuadTree<AlphaCell*>	fog_quadtree;			/*Contains the same data as the alpha layer but organized in a quadtree*/

	uint alpha_layer_width = 0;		/*Number of cells in the fog width*/
	uint alpha_layer_height = 0;	/*Number of cells in the fog height*/



	//Timer that count last update time
	j1Timer		update_timer;

	std::list<Entity*> entities_dinamic_update;
	std::vector<Entity*> entities_static_update;
	std::vector<Entity*> entities_release;

	std::vector<AlphaCell*> cells_in_screen;

public:

	//Boolean true when fog is ready to be updated
	uint				alpha_cell_size = 0;
	bool				fog_update = true;

	void				GenerateFogOfWar();
	void				CollectFogCells();

	FOG_TYPE			GetFogID(int x, int y)const;

	std::vector<AlphaCell*> ClearAlphaLayer(const Circle zone, unsigned short alpha = 0);
	void					ClearFogLayer(const Circle zone, FOG_TYPE type);

	void	CheckEntityFog(Entity* target);
	void	ReleaseEntityFog(Entity* target);
};

#endif