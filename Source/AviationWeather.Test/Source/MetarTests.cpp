/**********************************************************************************
 *                                                                                *
 * Copyright (c) 2015 Steven Frost, Orion Lyau. All rights reserved.              *
 *                                                                                *
 * This source is subject to the MIT License.                                     *
 * See http://opensource.org/licenses/MIT                                         *
 *                                                                                *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,    *
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED          *
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.         *
 *                                                                                *
 * NOT TO BE USED AS A SOLE SOURCE OF INFORMATION FOR FLIGHT CRITICAL OPERATIONS. *
 *                                                                                *
 **********************************************************************************/

#include "AviationWeather.TestPch.h"

#include <string>

#include <AviationWeather/converters.h>
#include <AviationWeather/metar.h>
#include <AviationWeather/types.h>

#include "framework.h"

 //-----------------------------------------------------------------------------

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

//-----------------------------------------------------------------------------

namespace aw
{
namespace test
{

//-----------------------------------------------------------------------------

TEST_CLASS(MetarTests)
{
public:
    TEST_METHOD(METAR_VariableWinds);
    TEST_METHOD(METAR_FractionalVisibility);
    TEST_METHOD(METAR_Visibility_Comparison);
    TEST_METHOD(METAR_Phenomena);
};

//-----------------------------------------------------------------------------

void MetarTests::METAR_VariableWinds()
{
    aw::metar::metar_info m1("KSFO 112056Z VRB03KT 10SM FEW010 SCT180 22/12 A2994 RMK AO2 SLP137 T02220122 58006");

    Assert::IsTrue(m1.wind_group.is_variable());
    Assert::AreEqual(UINT16_MAX, m1.wind_group.direction);
    Assert::AreEqual(static_cast<uint8_t>(3), m1.wind_group.wind_speed);
    Assert::AreEqual(static_cast<uint8_t>(0), m1.wind_group.gust_speed);
    Assert::AreEqual(static_cast<uint8_t>(0), m1.wind_group.gust_factor());
    Assert::AreEqual(UINT16_MAX, m1.wind_group.variation_lower);
    Assert::AreEqual(UINT16_MAX, m1.wind_group.variation_upper);

    aw::metar::metar_info m2("KRHV 101451Z 29014G18KT 10SM SKC 27/13 A2990");

    Assert::IsFalse(m2.wind_group.is_variable());
    Assert::AreEqual(static_cast<uint16_t>(290), m2.wind_group.direction);
    Assert::AreEqual(static_cast<uint8_t>(14), m2.wind_group.wind_speed);
    Assert::AreEqual(static_cast<uint8_t>(18), m2.wind_group.gust_speed);
    Assert::AreEqual(static_cast<uint8_t>(4), m2.wind_group.gust_factor());
    Assert::AreEqual(UINT16_MAX, m2.wind_group.variation_lower);
    Assert::AreEqual(UINT16_MAX, m2.wind_group.variation_upper);

    aw::metar::metar_info m3("KRHV 101850Z VRB02G06KT 10SM SKC 27/13 A2990");

    Assert::IsTrue(m3.wind_group.is_variable());
    Assert::AreEqual(UINT16_MAX, m3.wind_group.direction);
    Assert::AreEqual(static_cast<uint8_t>(2), m3.wind_group.wind_speed);
    Assert::AreEqual(static_cast<uint8_t>(6), m3.wind_group.gust_speed);
    Assert::AreEqual(static_cast<uint8_t>(4), m3.wind_group.gust_factor());
    Assert::AreEqual(UINT16_MAX, m3.wind_group.variation_lower);
    Assert::AreEqual(UINT16_MAX, m3.wind_group.variation_upper);

    aw::metar::metar_info m5("KRHV 112150Z 22512G15KT 200V250 10SM SKC 27/13 A2990");

    Assert::IsTrue(m5.wind_group.is_variable());
    Assert::AreEqual(static_cast<uint16_t>(225), m5.wind_group.direction);
    Assert::AreEqual(static_cast<uint8_t>(12), m5.wind_group.wind_speed);
    Assert::AreEqual(static_cast<uint8_t>(15), m5.wind_group.gust_speed);
    Assert::AreEqual(static_cast<uint8_t>(3), m5.wind_group.gust_factor());
    Assert::AreEqual(static_cast<uint16_t>(200), m5.wind_group.variation_lower);
    Assert::AreEqual(static_cast<uint16_t>(250), m5.wind_group.variation_upper);
}

//-----------------------------------------------------------------------------

void MetarTests::METAR_FractionalVisibility()
{
    aw::metar::metar_info m1("KSFO 121156Z 28005KT M1/4SM FEW006 15/12 A3000");

    Assert::AreEqual(aw::distance_unit::statute_miles, m1.visibility_group.unit);
    Assert::AreEqual(aw::metar::visibility_modifier_type::less_than, m1.visibility_group.modifier);
    Assert::AreEqual(1.0 / 4.0, m1.visibility_group.distance, 0.01);

    aw::metar::metar_info m2("KSFO 121156Z 28005KT 1/2SM FEW006 15/12 A3000");

    Assert::AreEqual(aw::distance_unit::statute_miles, m2.visibility_group.unit);
    Assert::AreEqual(aw::metar::visibility_modifier_type::none, m2.visibility_group.modifier);
    Assert::AreEqual(1.0 / 2.0, m2.visibility_group.distance, 0.01);

    aw::metar::metar_info m3("KSFO 121156Z 28005KT 1 1/2SM FEW006 15/12 A3000");

    Assert::AreEqual(aw::distance_unit::statute_miles, m3.visibility_group.unit);
    Assert::AreEqual(aw::metar::visibility_modifier_type::none, m3.visibility_group.modifier);
    Assert::AreEqual(1.0 + (1.0 / 2.0), m3.visibility_group.distance, 0.01);

    aw::metar::metar_info m4("KSFO 121156Z 28005KT 15SM FEW006 15/12 A3000");

    Assert::AreEqual(aw::distance_unit::statute_miles, m4.visibility_group.unit);
    Assert::AreEqual(aw::metar::visibility_modifier_type::none, m4.visibility_group.modifier);
    Assert::AreEqual(15.0, m4.visibility_group.distance, 0.01);
}

//-----------------------------------------------------------------------------

void MetarTests::METAR_Visibility_Comparison()
{
    const double distance = 10.0;

    aw::metar::visibility v1(distance, aw::distance_unit::statute_miles, aw::metar::visibility_modifier_type::none);
    aw::metar::visibility v2(distance, aw::distance_unit::statute_miles, aw::metar::visibility_modifier_type::less_than);
    aw::metar::visibility v3(distance - 1.0, aw::distance_unit::statute_miles, aw::metar::visibility_modifier_type::none);
    aw::metar::visibility v4(distance - 1.0, aw::distance_unit::statute_miles, aw::metar::visibility_modifier_type::less_than);

    // v1, v2
    Assert::IsFalse(v1 == v2); Assert::IsTrue(v1 != v2);
    Assert::IsFalse(v1 <= v2); Assert::IsTrue(v1 >= v2);
    Assert::IsFalse(v1 < v2);  Assert::IsTrue(v1 > v2);

    // v1, v3
    Assert::IsFalse(v1 == v3); Assert::IsTrue(v1 != v3);
    Assert::IsFalse(v1 <= v3); Assert::IsTrue(v1 >= v3);
    Assert::IsFalse(v1 < v3);  Assert::IsTrue(v1 > v3);

    // v1, v4
    Assert::IsFalse(v1 == v4); Assert::IsTrue(v1 != v4);
    Assert::IsFalse(v1 <= v4); Assert::IsTrue(v1 >= v4);
    Assert::IsFalse(v1 < v4);  Assert::IsTrue(v1 > v4);


    // v2, v1
    Assert::IsFalse(v2 == v1); Assert::IsTrue(v2 != v1);
    Assert::IsTrue(v2 <= v1);  Assert::IsFalse(v2 >= v1);
    Assert::IsTrue(v2 < v1);   Assert::IsFalse(v2 > v1);

    // v2, v3
    Assert::IsFalse(v2 == v3); Assert::IsTrue(v2 != v3);
    Assert::IsFalse(v2 <= v3); Assert::IsTrue(v2 >= v3);
    Assert::IsFalse(v2 < v3);  Assert::IsTrue(v2 > v3);

    // v2, v4
    Assert::IsFalse(v2 == v4); Assert::IsTrue(v2 != v4);
    Assert::IsFalse(v2 <= v4); Assert::IsTrue(v2 >= v4);
    Assert::IsFalse(v2 < v4);  Assert::IsTrue(v2 > v4);


    // v3, v1
    Assert::IsFalse(v3 == v1); Assert::IsTrue(v3 != v1);
    Assert::IsTrue(v3 <= v1);  Assert::IsFalse(v3 >= v1);
    Assert::IsTrue(v3 < v1);   Assert::IsFalse(v3 > v1);

    // v3, v2
    Assert::IsFalse(v3 == v2); Assert::IsTrue(v3 != v2);
    Assert::IsTrue(v3 <= v2);  Assert::IsFalse(v3 >= v2);
    Assert::IsTrue(v3 < v2);   Assert::IsFalse(v3 > v2);

    // v3, v4
    Assert::IsFalse(v3 == v4); Assert::IsTrue(v3 != v4);
    Assert::IsFalse(v3 <= v4); Assert::IsTrue(v3 >= v4);
    Assert::IsFalse(v3 < v4);  Assert::IsTrue(v3 > v4);


    // v4, v1
    Assert::IsFalse(v4 == v1); Assert::IsTrue(v4 != v1);
    Assert::IsTrue(v4 <= v1);  Assert::IsFalse(v4 >= v1);
    Assert::IsTrue(v4 < v1);   Assert::IsFalse(v4 > v1);

    // v4, v2
    Assert::IsFalse(v4 == v2); Assert::IsTrue(v4 != v2);
    Assert::IsTrue(v4 <= v2);  Assert::IsFalse(v4 >= v2);
    Assert::IsTrue(v4 < v2);   Assert::IsFalse(v4 > v2);

    // v4, v3
    Assert::IsFalse(v4 == v3); Assert::IsTrue(v4 != v3);
    Assert::IsTrue(v4 <= v3);  Assert::IsFalse(v4 >= v3);
    Assert::IsTrue(v4 < v3);   Assert::IsFalse(v4 > v3);
}

//-----------------------------------------------------------------------------

void MetarTests::METAR_Phenomena()
{
    aw::metar::metar_info m1("KCCR 181953Z 28011KT 10SM FU CLR 27/12 A2990 RMK AO2 SLP110 T02720122");

    Assert::AreEqual(1U, static_cast<uint32_t>(m1.weather_group.size()));
    auto it = m1.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::smoke, it->phenomena.front());
        
    aw::metar::metar_info m2("KRHV 181947Z 33008/KT 6SM HZ SKC 29/12 A2992");

    Assert::AreEqual(1U, static_cast<uint32_t>(m2.weather_group.size()));
    it = m2.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::haze, it->phenomena.front());

    aw::metar::metar_info m3("KSTS 181253Z AUTO 00000KT 10SM -RA HZ FU OVC007 14/12 A2990 RMK AO2 RAB30 SLP116 P0000 T01390122");

    Assert::AreEqual(3U, static_cast<uint32_t>(m3.weather_group.size()));
    it = m3.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::light, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::rain, it->phenomena.front());
    it++;
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::haze, it->phenomena.front());
    it++;
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::smoke, it->phenomena.front());

    aw::metar::metar_info m4("KSTS 182253Z 19008KT 9SM FU HZ CLR 25/13 A2988 RMK AO2 SLP112 T02500133");

    Assert::AreEqual(2U, static_cast<uint32_t>(m4.weather_group.size()));
    it = m4.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::smoke, it->phenomena.front());
    it++;
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::haze, it->phenomena.front());

    aw::metar::metar_info m5("KWVI 171553Z AUTO 00000KT 1 1/4SM BR OVC002 17/15 A2989 RMK AO2 SLP122 T01670150");

    Assert::AreEqual(1U, static_cast<uint32_t>(m5.weather_group.size()));
    it = m5.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::mist, it->phenomena.front());

    aw::metar::metar_info m6("KWVI 171453Z AUTO 00000KT 1/4SM FG VV002 16/15 A2989 RMK AO2 SLP119 T01610150 53007");

    Assert::AreEqual(1U, static_cast<uint32_t>(m6.weather_group.size()));
    it = m6.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::fog, it->phenomena.front());

    aw::metar::metar_info m7("KORD 190151Z 19010KT 5SM TSRA BR FEW027 BKN048CB OVC090 21/19 A2971 RMK AO2 PK WND 18028/0112 SLP057 FRQ LTGICCG OHD TS OHD MOV NE P0023 T02060194");

    Assert::AreEqual(2U, static_cast<uint32_t>(m7.weather_group.size()));
    it = m7.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::thunderstorm, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::rain, it->phenomena.front());
    it++;
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::mist, it->phenomena.front());
}

//-----------------------------------------------------------------------------

} // namespace test
} // namespace aw
