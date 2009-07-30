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

public class PlotData
{
    double[] data;
    String[] time;
    double[] hoursFromZero;
    int count;
    DateTime zeroDateTime;

    public PlotData()
    {
    }

	public PlotData(int count)
	{
        this.count = count;
        data = new double[count];
        time = new String[count];
        hoursFromZero = new double[count];
	}

    public void add(int i, double data, String time)
    {
        this.data[i] = data;
        this.time[i] = time;
    }

    public void setData(double[] data)
    {
        this.data = data;
    }

    public double[] getData()
    {
        return data;
    }

    public void setHoursFromZero(double[] hoursFromZero)
    {
        this.hoursFromZero = hoursFromZero;
    }

    public double[] getHoursFromZero()
    {
        return hoursFromZero;
    }

    public void setHoursFromZero(DateTime zeroDateTime)
    {
        DateTime dt;
        TimeSpan ts;
        int year, month, day, hour, minute, second;

        this.zeroDateTime = zeroDateTime;
        for (int i = 0; i < count; i++)
        {
            // Format for string from database : dd/mm/yyyy HH:mm:ss
            year = Int32.Parse(time[i].Substring(6, 4));
            month = Int32.Parse(time[i].Substring(3, 2));
            day = Int32.Parse(time[i].Substring(0, 2));
            hour = Int32.Parse(time[i].Substring(11, 2));
            minute = Int32.Parse(time[i].Substring(14, 2));
            second = Int32.Parse(time[i].Substring(17, 2));
            dt = new DateTime(year, month, day, hour, minute, second);

            ts = dt - zeroDateTime;
            hoursFromZero[i] = ts.TotalHours;
        }
    }
}
