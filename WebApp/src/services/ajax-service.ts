/**
 * AjaxServiceController
 */
export class AjaxServiceController {

  /**
   * baseUrl
   */
  //public baseUrl: string = location.origin + '/';
  public baseUrl: string = 'http://wordclock.local/';

  /**
   * getSettings
   */
  getSettings = async (): Promise<any> => {
    return fetch(this.baseUrl + 'settings.json')
      .then(response => {
        if (!response.ok) {
          throw new Error(response.statusText);
        }
        return response.json();
      })
      .then(data => {
        return data;
      });
  };

  /**
   * setSettings
   *
   * @param field
   * @param value
   */
  setSettings = async (field: string, value: any): Promise<any> => {
    if ((typeof field !== "undefined" || field !== null) && (typeof value !== "undefined" || value !== null)) {
      let data = {};
      data[field] = value;
      fetch(this.baseUrl + 'update.json', {
        method: 'POST',
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8'
        },
        body: JSON.stringify(data)
      })
        .then(response => {
          if (!response.ok) {
            throw new Error(response.statusText);
          }
          return response.json();
        })
        .then(data => {
          return data;
        });
    }
  };
}

export const AjaxService = new AjaxServiceController();
