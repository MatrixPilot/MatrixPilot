using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace XplaneTesting
{
    class XplanePackets
    {

        
    enum LOCATION_SELECT{
      LOCATION_SELECT_ACTUAL = 0,
      LOCATION_SELECT_SFO,
      LOCATION_SELECT_BAYLANDS,
    };

    public const double BAYLANDS_LAT = (37.4124664);
    public const double BAYLANDS_LONG=(-121.9950467);
    public const double BAYLANDS_ALT=(2.0);
    public const double SFO_LAT=(37.622118);
    public const double SFO_LONG=(-122.381172);
    public const double SFO_ALT=(10.0);


    // For info about the X Plane UDP protocol, see this file in your xplane folder
    // C:\X-Plane 10\Instructions\Sending Data to X-Plane.html

    public const uint strDIM = 500;
    public const uint vehDIM = 20;
    public const uint path_rel_SIZE = 150;
    public const uint XPLANE_PACKET_HEADER_SIZE = 5;
    public const uint att_file_QTY = 24;
    public const uint att_file_SIZE = 40;



    class data_struct
    {
        public int index; // data index, the index into the list of variables you can output from the Data Output screen in X-Plane.
      public float[] data = new float[8]; // the up to 8 numbers you see in the data output screen associated with that selection.. many outputs do not use all 8, though.
    }

    class soun_struct
    { // play any sound
        public float freq, vol;
      public char[] path = new char[strDIM];
    }

    class dref_struct
    {
        public float var;
      public char[] dref_path = new char[strDIM];
    }

    class papt_struct
    {
        public char[] apt_ID = new char[5]; // ID to go to, null-termed
      public int type_start; // 601 for takeoff, 602 for final approach, 603 for far-away final approach
      public int lcl_rwy_ram; // which runway or ramp-start location to use, starting at 0 for each airport
      public int backwards; // 0 or 1, depending on if you want to appraoch from the other direction
    }


    class vehN_struct
    {

      public int p;
      public char[] path_rel = new char[path_rel_SIZE];
      public char[][] att_file = new char[att_file_QTY][];
    }


    class VEH1_struct
    { // byte-align 4
        public int unknown;
        public int p; // this is the plane you wish to control.. 0 for your plane, but you can enter up to 9 here
        public double[] lat_lon_ele = new double[3];
        public float[] psi_the_phi = new float[3];
        public float[] gear_flap_vect = new float[3];
    }


    class vehA_struct
    {
        public int num_p;

        public int unknown1;
        public double[][] lat_lon_ele = new double[vehDIM][];
        public float[][] psi_the_phi = new float[vehDIM][];
        public float[][] gear_flap_vect = new float[vehDIM][];
        public int unknown2;
        public double lat_view, lon_view, ele_view;
        public float psi_view, the_view, phi_view;
    }
        
    class objN_struct
    { // object name: draw any object in the world in the sim
      public int index;
      public char[] path = new char[strDIM];
    }
        

    } // class XplanePackets
} // namespace
