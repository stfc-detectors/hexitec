using System;
using System.Data;
using System.Configuration;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Xml.Linq;

public interface Plotter
{
    void plotData(PlotData recent_scada, PlotData historical_scada, PlotData last_forecast, PlotData merged_forecast, String title);
}
