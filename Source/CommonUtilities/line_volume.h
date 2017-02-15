#pragma once
#include "line.h"

namespace CU
{
	template<typename TYPE>
	class LineVolume
	{
	public:


		// Initializes the volume with a list of lines.
		//
		LineVolume(const CU::GrowingArray<Line<TYPE>> &someLines);

		// Adds a line to the volume
		//
		void AddLine(const Line<TYPE> &aLine);

		// Removes a line from the volume.
		//
		void RemoveLine(const Line<TYPE> &aLine);

		// Returns true if the point is inside of all lines in the volume.
		//
		bool Inside(const Point2<TYPE> &aPoint) const;

	private:    // Internal representation of the line goes here
		CU::GrowingArray<Line<TYPE>> myLines;

	};

	template<typename TYPE>

	LineVolume<TYPE>::LineVolume(const CU::GrowingArray<Line<TYPE>> &someLines)
	{
		myLines = someLines;
	}

	template<typename TYPE>
	void LineVolume<TYPE>::AddLine(const Line<TYPE> &aLine)
	{
		myLines.Add(aLine);
	}

	template<typename TYPE>
	void LineVolume<TYPE>::RemoveLine(const Line<TYPE> &aLine)
	{
		myLines.RemoveCyclic(aLine);
	}

	template<typename TYPE>
	bool LineVolume<TYPE>::Inside(const Point2<TYPE> &aPoint) const
	{
		for (int i = 0; i < myLines.Size(); ++i)
		{
			if (myLines[i].IsInside(aPoint) == false)
			{
				return false;
			}
		}
		return true;
	}
}