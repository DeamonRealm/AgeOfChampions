#ifndef _FOG_OF_WAR_
#define _FOG_OF_WAR_

#include "j1Module.h"
#include "Iso_Primitives.h"
#include "Quadtree.h"
#include "j1Timer.h"

#define ALPHA_LIMIT 255			/* Values between 0 & 255 */
#define MID_ALPHA 150			/* Values between 0 & 255 */
#define	DIVISIONS_PER_PIXELS 60	/* Values between 20 & 60 in debug for >60 fps */ /* Values between 20 & 130 in release for >60 fps */
#define RENDER_MARGIN 80		/* Values between 0 & INF */
#define	UPDATE_RATE	100			/* DT between fog updates for optimize the process */

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

private:

	FOG_TYPE*				fog_layer = nullptr;	/*Layer that contains fog types*/
	AlphaCell*				alpha_layer = nullptr;	/*Layer that contains fog alpha values*/
	QuadTree<AlphaCell*>	fog_quadtree;			/*Contains the same data as the alpha layer but organized in a quadtree*/

	uint alpha_layer_width = 0;		/*Number of cells in the fog width*/
	uint alpha_layer_height = 0;	/*Number of cells in the fog height*/

	uint alpha_cell_size = 0;

	//Timer that count last update time
	j1Timer		update_timer;

public:

	//Boolean true when fog is ready to be updated
	bool				fog_update = true;

	void				GenerateFogOfWar();

	FOG_TYPE			GetFogID(int x, int y)const;

	void				ClearAlphaLayer(const Circle zone, unsigned short alpha = 0);
	void				ClearFogLayer(const Circle zone, FOG_TYPE type);
};

#endif