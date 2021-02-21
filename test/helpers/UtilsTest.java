// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
package helpers;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class UtilsTest {

  /**
   * Test of prettyPrintDouble method, of class Utils.
   */
  @Test
  public void testPrettyPrintDouble() {
    assertEquals("2.3M", Utils.prettyPrintDouble(2300000));
    assertEquals("1.0M", Utils.prettyPrintDouble(999999));
    assertEquals("1.0k", Utils.prettyPrintDouble(1000));
    assertEquals("1.0k", Utils.prettyPrintDouble(999));
    assertEquals("1.0k", Utils.prettyPrintDouble(995));
    assertEquals("990", Utils.prettyPrintDouble(994));
    assertEquals("100m", Utils.prettyPrintDouble(0.1));
    assertEquals("1.6m", Utils.prettyPrintDouble(0.0016));
    assertEquals("640μ", Utils.prettyPrintDouble(6.45E-4));
    assertEquals("0", Utils.prettyPrintDouble(0));
    assertEquals("-1.0k", Utils.prettyPrintDouble(-995));
    assertEquals("1.0E-30", Utils.prettyPrintDouble(1E-30));
    assertEquals("1.0E+30", Utils.prettyPrintDouble(1E30));
    assertEquals("0", Utils.prettyPrintDouble(1E-40));
  }
  
}
